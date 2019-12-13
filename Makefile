## - make all to build a binary called 'mwc'
## - make tests to test this binary using the 'test.sh' script in the
##   tests folder
## If your executable depends on other '.c' files, add them line 14
CC = gcc

all: bin

.PHONY: bin
bin: mwc

%.c: %.h

mwc: main.c
	$(CC) -O3 -o $@ -pthread $^

.PHONY: tests
tests: mwc
	BIN=./$< tests/test.sh
