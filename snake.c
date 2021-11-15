#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>

int btn; // pressed button variable
int dir [2]; // directions 0; 1; -1; set by
short int score = 0;
short int length = 0;
char gameover = 0; // exit codes: 0 game running; 1 manual exit; 2 self-eating
const short int vsize = 10;
const short int hsize = 20;
int maxlen = (vsize-2)*(hsize-2);
short int foord [2]; //food coordinates

void redraw (short int snake [2][maxlen], WINDOW *gw, short int foord []); //redrawing frame
short int *spawnfood (short int snake [2][maxlen]); //food spawning
bool kproc (int btn, int dir [1]); //key processing
void mvsnake (short int snake [2][maxlen]); //snake movement
void bordercross (short int snake [2][maxlen]); //reaction on border approach (currently spawning from opposite side)
bool is_eating (short int snake [2][maxlen]); //means self-eating



int main () {

  initscr (); //initializing ncurses
  noecho ();
  keypad (stdscr, TRUE);
  nodelay (stdscr, TRUE);
  curs_set (0);
  WINDOW *gw = newwin (vsize, hsize, 1, 0); //gw - game window
  WINDOW *sw = newwin (1, hsize, 0, 0); //sw - score window
  mvprintw (11, 0, "Use arrow keys to move\npress backspace to exit game");
  mvwprintw (sw, 0, 0, "Score: %08d", score);
  refresh ();
  wrefresh (sw);
  short int snake [2][maxlen];
  snake [0][0] = (vsize-2)/2;
  snake [1][0] = (hsize-2)/2;
  spawnfood (snake); // game init
  redraw (snake, gw, foord); //game init
  while (!gameover) {
    if ((btn = getch()) != ERR) {
      if (kproc (btn, dir)) {
        gameover = 1;
        break;
      }
    };
    mvsnake (snake);
    bordercross (snake);
    if (snake [0][0] == foord [0] && snake [1][0] == foord [1]) {
      ++length; ++score;
      spawnfood (snake);
      mvwprintw (sw, 0, 0, "Score: %08d", score);
      wrefresh (sw);
    }
    else if (is_eating (snake)) {
      gameover = 2;
      break;
    }
    redraw (snake, gw, foord);
    sleep (1);
  }
  switch (gameover) {
    case 1 : mvwprintw(gw, 5, 0, "Press any key\nto exit the game\n"); break;
    case 2 : mvwprintw(gw, 5, 0, "Oh no!\nseems your snake\nate itself\n"); break;
  }
  wrefresh (gw);
  getchar ();
  endwin();
  return 0;

}

void redraw (short int snake [2][maxlen], WINDOW *gw, short int foord []) {

  werase (gw);
  mvwaddch (gw, foord [0], foord [1], '*');
  for (int i = 1; i <= length; i++){
    mvwaddch (gw, snake [0][i], snake [1][i], 'O');
  }
  mvwaddch (gw, snake [0][0], snake [1][0], '@');
  box (gw, 0, 0);
  wrefresh (gw);

}

short int *spawnfood (short int snake [2][maxlen]) {

  srand (time(NULL));
  foord [0] = 1+rand()%(vsize-2);
  foord [1] = 1+rand()%(hsize-2);
  return foord;

}

void mvsnake (short int snake [2][maxlen]) {

  for (int i=length; i > 0; i--) {
    snake [0][i] = snake [0][i-1];
    snake [1][i] = snake [1][i-1];
  }
  snake [0][0] += dir [0];
  snake [1][0] += dir [1];

}

void bordercross (short int snake [2][maxlen]) {

  if (snake [0][0] == (vsize-1)) snake [0][0] = 1;
      else if (snake [0][0] <= 0) snake [0][0] = vsize - 2;
      else if (snake [1][0] == (hsize-1)) snake [1][0] = 1;
      else if (snake [1][0] <= 0) snake [1][0] = hsize - 2;

}

bool is_eating (short int snake [2][maxlen]) {

  bool ext = false; //exit status
  for (int i = 1; i <= length; i++) {
      if (snake [0][i] == snake [0][0] && snake [1][i] == snake [1][0]) {
      ext = true;
      break;
      }
  }
  return ext;

}

bool kproc (int btn, int dir[2]) {

  bool go = false; //backspace reaction
  if (btn == KEY_BACKSPACE) go = true;
  else if (btn == KEY_UP&& dir [0] != 1) {dir [0] = -1; dir [1] = 0;}
  else if (btn == KEY_DOWN&& dir [0] != -1) {dir [0] = 1; dir [1] = 0;}
  else if (btn == KEY_LEFT&& dir [1] != 1) {dir [0] = 0; dir [1] = -1;}
  else if (btn == KEY_RIGHT&& dir [1] != -1) {dir [0]= 0; dir [1] = 1;}
  return go;

}
