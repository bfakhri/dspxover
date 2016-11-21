all: jackfilter.c filter.c filter.h
	gcc -o dsp.exec jackfilter.c filter.c `pkg-config --cflags --libs jack` -std=c99
