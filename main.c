#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>

#include "screen.h"
#include "buffer.h"
#include "util.h"

// screen info 
struct{
	int x, y;
}scr;

extern buffer_t main_buf;

void buffer_addch(char* b, int* p, char c){
	int end = strlen(b);
	b[end+1] = '\0';
	for(int i = end; i > *p; i--){
		b[i] = b[i-1];
	}
	b[*p] = c;
	// now update the pointer
	(*p)++;
	return;

}

void buffer_rmch(char* b, int* p){	
	int i = *p;
	if(!b[*p]){
		b[*p-1] = '\0';
		(*p)--;
	}else{
		while(b[i]){
			b[i] = b[i+1]; i++;
		}
	}
	return;
}

void process(){	
	int c = NULL;
	FILE* f = NULL;
	char b[1024];
	int n = 1024;
	memset(b, 0, sizeof(char)*1024);
	int p = 0;	// edit position in buffer - should point to the next character
			// we will replace
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
				f = popen(b, "r");
				b[0] = NULL; p = 0;
				fgets(b, 1024, f);
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
		int x, y;
		getyx(stdscr, y, x);
		move(y, 0);
		clrtoeol();
		printw("%s", b);
		move(y, p);
		refresh();
	}
}

int main(){
	screen_init(&scr.x, &scr.y);	
	refresh();
	buffer_from_file(fopen("test.sh", "r"), &main_buf);
	process();
	screen_end();
	return 0;
}
