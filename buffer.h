#include <ncurses.h>
#include <stdlib.h>
#include <gmodule.h>

typedef struct buffer_t{
	GList* buf_root;	// buffer is implemented as doubly linked list
	/** TODO **/
}buffer_t;

extern buffer_t main_buf;

void buffer_from_file(FILE* stream, buffer_t* buf);
