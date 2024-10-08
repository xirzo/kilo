/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** data ***/
struct termios initial_termios;

/*** terminal ***/
void die(const char *s) {
  perror(s);
  exit(1);
}

void disablerawmode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &initial_termios) == -1)
    die("tcsetattr");
}

void enablerawmode() {
  if (tcgetattr(STDIN_FILENO, &initial_termios) == -1)
    die("tcgetattr");

  atexit(disablerawmode);

  struct termios raw = initial_termios;
  raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_lflag &= ~(OPOST);
  raw.c_lflag &= ~(CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

/*** init ***/
int main() {
  enablerawmode();

  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
      die("read");

    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }

    if (c == 'q')
      break;
  }

  return 0;
}
