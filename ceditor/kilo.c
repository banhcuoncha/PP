                                        /*** includes ***/
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>

                                        /*** defines ***/
struct editorConfig {
    struct termios orig_termios;
    int screen_rows;
    int screen_cols;
};

struct editorConfig E;
#define CTRL_KEY(k) ((k) & 0x1f)
                                        /*** terminal ***/
void die(const char* s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}
void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) {
        die("tcsetattr");
    }
}

void enableRawMode() {
    // Save the original terminal settings
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) {
        die("tcgetattr");
    }
    atexit(disableRawMode);

    struct termios raw = E.orig_termios;
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
int getCursorPosition(int* rows, int* cols) {
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) { return -1; }

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1 ) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
    
    return 0;
}
int getWindowSize(int* rows, int* columns) {
    struct winsize ws;

    if (1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) { return -1; }
        
        return getCursorPosition(rows, columns);
    } else {
        *columns = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}
                                    /*** append buffer ***/
struct abuf {
    char *b;
    int len;
};
#define ABUF_INIT {NULL, 0}

 void abAppend(struct abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b, ab->len + len);

    if (!new) { return; }
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
 }

 void abFree(struct abuf *ab) {
    free(ab->b);
 }
                                /*** output ***/
void editorDrawRows(struct abuf *ab) {
    int y;
    for (y = 0; y < E.screen_rows; y++) {
        abAppend(ab, "~", 1);
        
        if (y < E.screen_rows - 1) {
            abAppend(ab, "\r\n", 2);
        }
    }
}
                                    /*** input ***/
void editorRefreshScreen() {
    struct abuf ab = ABUF_INIT;


    abAppend(&ab, "\x1b[?25l", 6);
    abAppend(&ab, "\x1b[2J", 4);
    abAppend(&ab, "\x1b[H", 3);

    editorDrawRows(&ab);
    // Reposisiton cursor back up to the top left corner
    abAppend(&ab, "\x1b[H", 3);
    abAppend(&ab, "\x1b[?25h", 6);
    
    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
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
void initEditor() {
    if (getWindowSize(&E.screen_rows, &E.screen_cols)) {
        die("getWindowSize");
    }
}
int main() {
    enableRawMode();
    initEditor();
    while (1) {
        editorRefreshScreen();
        editorProcessKeyPress();
    }
    return 0;
}
