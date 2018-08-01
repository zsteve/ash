#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>

#include "screen.h"
#include "buffer.h"
#include "util.h"
#include "process.h"

// screen info 
struct{
	int x, y;
}scr;

extern buffer_t main_buf;

void newline(){
	int x, y;
	getyx(stdscr, y, x);
	move(y+1, x);
}

void parse_cmd(char* command, int* argc, char** argv, int max_args){
	char* tok;
	char t[2] = " ";
	int i = 0;
	tok = strtok(command, t);
	
	while(tok != NULL){
		argv[i] = malloc((strlen(tok)+1)*sizeof(char));
		strcpy(argv[i], tok);
		i++;
		tok = strtok(NULL, t);
	}
	argv[i] = NULL;
}

void clobber_argv(int argc, char** argv){
	for(int i = 0; i < argc; i++){
		free(argv[i]);
	}
}

void process(){	
	int c = NULL;
	FILE* f = NULL;
	char b[1024];
	int n = 1024;
	memset(b, 0, sizeof(char)*1024);
	int p = 0;	// edit position in buffer - should point to the next character
				// we will replace
	
	
	cell_list_t cell_list = cell_list_create();
	cell_t* t = cell_list_append(&cell_list);

	cell_makewindow(t, 4, scr.x, 1, 0);

	while(c = getch()){
		switch(c){
			case CTRL('d'):
				return;
				break;
			case KEY_DC:
			case 127:
				buffer_rmch(b, &p);
				break;
			case KEY_ENTER:
			case 10:
				//run(b);
				b[0] = '\0'; p = 0;
				newline();
				break;
			case KEY_LEFT:
				if(p > 0) p--;
				break;
			case KEY_RIGHT:
				if(p < 1024) p++;
				break;
			default:
				buffer_addch(b, &p, c);
				break;
		}
		/*int x, y;
		getyx(stdscr, y, x);
		move(y, 0);
		clrtoeol();
		printw("%s", b);
		move(y, p);*/
		refresh();
	}
}

int main(){
	screen_init(&scr.x, &scr.y);
	refresh();
	process();
	screen_end();
	return 0;
}
