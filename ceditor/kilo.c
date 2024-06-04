/*** includes ***/
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
/*** defines ***/
struct termios orig_termios;
#define CTRL_KEY(k) ((k) & 0x1f)
/*** terminal ***/
void die(const char* s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}
void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        die("tcsetattr");
    }
}

void enableRawMode() {
    // Save the original terminal settings
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        die("tcgetattr");
    }
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    // Turn off CTRL-Q, CTRL-S, CTRL-M signals
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP| ICRNL | IXON);
    // Turn off all output processing
    raw.c_oflag &= ~(OPOST);
    // Miscellaneous
    raw.c_cflag |= (CS8);
    /* Turn off echo & canonical mode && catch ctrl signals
    ISIG: CTRL-C & D, IEXTEN: CTRL-V
    */
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    // Adding timeout for read
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        die("tcsetattr");
    }
}
char editorReadKey() {
    int nread;
    char c;
    while ((nread  = read(STDIN_FILENO, &c, 1) != 1)) {
        if (nread == -1 && errno != EAGAIN) { die("read"); }
    }
    return c;
}
/*** output ***/
void editorDrawRows() {
    int y;
    for (y = 0; y < 24; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}
/*** input ***/
void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorDrawRows();
    // Reposisiton cursor back up to the top left corner
    write(STDOUT_FILENO, "\x1b[H", 3);
}
void editorProcessKeyPress() {
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('w'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}
/** init **/
int main() {
    enableRawMode();
    
    while (1) {
        editorRefreshScreen();
        editorProcessKeyPress();
    }
    return 0;
}
