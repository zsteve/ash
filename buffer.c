#include "buffer.h"
#include <ncurses.h>

buffer_t main_buf = { .buf_root = NULL };

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
	c->h = 0;
	return c;
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

void cell_list_append(cell_list_t* c){
	c->root = g_list_append(c->root, cell_create());
	return;
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

