#include "buffer.h"
#include "io.h"
#include "process.h"
#include <ncurses.h>


buffer_t main_buf = { .buf_root = NULL };

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

void buffer_from_file(FILE* stream, buffer_t* buf){
	char* read_buf = NULL;
	int n = 0;
	while(getline(&read_buf, &n, stream) != -1){
		buf->buf_root = g_list_append(buf->buf_root, (gpointer)read_buf);
		read_buf = NULL; n = 0;		
	}
}

cell_t* cell_create(){
	cell_t* c = (cell_t*)malloc(sizeof(cell_t));
	c->input_buf = (buffer_t){.buf_root = NULL};
	c->output_buf = (buffer_t){.buf_root = NULL};
	c->pid = NULL;
	c->win = NULL;
	return c;
}

int cell_makewindow(cell_t* c, int h, int w, int y, int x){
	if(c->win) return 0;	// window already exists
	c->win = newwin(h, w, y, x);
	wrefresh(c->win);
	// call cell window procedure
	if(fork()){
		// parent process
	}else{
		// child process
		cell_windowproc(c);
	}
	return 1;
}

void cell_windowproc(cell_t* c){
	int k = NULL;
	start_color();
	init_pair(1,COLOR_BLUE, COLOR_RED);
	wbkgd(c->win, COLOR_PAIR(1));
	wrefresh(c->win);
	while(k = getch()){
		switch(k){
			default:
				break;
		}
		int x, y;
		getyx(c->win, y, x);
		wmove(c->win, y, 0);
		wclrtoeol(c->win);
		wprintw(c->win, "Cell text");
		wrefresh(c->win);
		wmove(c->win, y, x);
	}
}

int cell_delwindow(cell_t* c){
	if(c->win) return 0;
	delwin(c->win); c->win = NULL;
	return 1;
}

void cell_destroy(cell_t* c){
	free(c);
	return;
}

cell_list_t cell_list_create(){
	return (cell_list_t){.root = NULL};
}

void cell_list_destroy(cell_list_t c, void (*f)(void*)){
	if(c.root){
		g_list_free_full(c.root, f);
	}
}

cell_t* cell_list_append(cell_list_t* c){
	c->root = g_list_append(c->root, cell_create());
	return cell_list_last(c);
}

cell_t* cell_list_last(cell_list_t* c){
	return (cell_t*)g_list_last(c->root)->data;
}

void run_cell(cell_t* c){
	run_cmd((char*)g_list_last(c->input_buf.buf_root)->data);
}

void run_cmd(char* command){
	// IMPORTANT: args must have last element NULL
	// allows for variadic functions
	int fdm, fds;
	make_pt(&fdm, &fds);

	/** parse commands **/

	if(fork()){
		// parent process
		char output[1024*1024];	// 1MB output buffer
		close(fds);
		while(1){
			int rc = read(fdm, output, sizeof(output)-1);
			if(rc > 0){
				output[rc] = '\0';
				printw("%s", output);
				refresh();
			}else{
				break;
			}
		}

	}else{
		// child process
		int rc = 0;

		struct termios slave_orig_term_settings;
		struct termios new_term_settings;
		
		close(fdm);
		
		rc = tcgetattr(fds, &slave_orig_term_settings);
		new_term_settings = slave_orig_term_settings;
		cfmakeraw(&new_term_settings);
		tcsetattr(fds, TCSANOW, &new_term_settings);

		close(0); close(1); close(2);
		dup(fds);
		dup(fds);
		dup(fds);
		setsid();
		ioctl(0, TIOCSCTTY, 1);

		system(command);
	}
}

