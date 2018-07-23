#include "screen.h"

int screen_init(int* x, int* y){
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, *y, *x);
	printw("bashnote");	
	mvchgat(0, 0, *x, A_REVERSE, COLOR_WHITE, NULL);
	move(1, 0);
	return 0;
}

void screen_end(){
	endwin();	
}

int screen_print(){
	// print title bar
	
	return 0;
}
