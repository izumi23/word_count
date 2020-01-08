CC = gcc

all: bin

bin: wiki

%.c: %.h

wiki: wiki.c
	$(CC) -O3 -o $@ $^
