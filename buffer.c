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
