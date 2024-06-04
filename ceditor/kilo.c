#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

struct termios orig_termios;

void die(const char* s) {
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
int main() {
    enableRawMode();
    
    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1) { die("read"); };
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') { break; }
    }
    return 0;
}
