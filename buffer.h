#include <ncurses.h>
#include <stdlib.h>
#include <gmodule.h>

typedef struct buffer_t{
	GList* buf_root;	// buffer is implemented as doubly linked list
	/** TODO **/
}buffer_t;

typedef struct cell_t{
	buffer_t input_buf;
	buffer_t output_buf;
	pid_t pid;	// pid of the attached process (NULL if none)
	int h;		// cell height (chars)

}cell_t; // single cell type

typedef struct cell_list_t{
	GList* root;
}cell_list_t;

extern buffer_t main_buf;

void buffer_from_file(FILE* stream, buffer_t* buf);

cell_t* cell_create();
void cell_destroy(cell_t* c);

cell_list_t cell_list_create();
void cell_list_destroy(cell_list_t c, void (*f)(void*)); 
void cell_list_append(cell_list_t* c);

void run_cell(cell_t* c);
void run_cmd(char* command);