main: main.c screen.c buffer.c
	gcc -g -o bin/main\
		-I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include\
		-L/usr/lib -lm -lglib-2.0\
		-lncurses main.c screen.c buffer.c
