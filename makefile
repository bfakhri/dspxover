all: test.c
	gcc -o myprog test.c `pkg-config --cflags --libs jack` -std=c99
