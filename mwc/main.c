#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>



void char_count (int argc, char **argv) {
  int i, count, f;
  char c;
  int total = 0;
  for (i=2; i<argc; i++) {
    count = 0;
    f = open(argv[i], O_RDONLY);
    while (read(f, &c, 1)) count++;
    close(f);
    printf("%d %s\n", count, argv[i]);
    total += count;
  }
  if (argc > 3) printf("%d total\n", total);
}



void line_count (int argc, char **argv) {
  int i, count, f;
  char c;
  int total = 0;
  for (i=2; i<argc; i++) {
    count = 0;
    f = open(argv[i], O_RDONLY);
    while (read(f, &c, 1)) {
      if (c == '\n') count++;
    }
    close(f);
    printf("%d %s\n", count, argv[i]);
    total += count;
  }
  if (argc > 3) printf("%d total\n", total);
}



void word_count (int argc, char **argv, int debut) {
  int i, count, f;
  int sep;
  char c;
  int total = 0;
  for (i=debut; i<argc; i++) {
    count = 0;
    sep = 1;
    f = open(argv[i], O_RDONLY);
    while (read(f, &c, 1)) {
      if (c == ' ' || c == '\n' || c == '\t') sep = 1;
      else if (sep) {sep = 0; count++;}
    }
    close(f);
    printf("%d %s\n", count, argv[i]);
    total += count;
  }
  if (argc-debut > 2) printf("%d total\n", total);
}



int main (int argc, char **argv) {
  if (argv[1][0] == '-') {
    char opt = argv[1][1];
    if (opt == 'c') char_count(argc, argv);
    else if (opt == 'l') line_count(argc, argv);
    else word_count(argc, argv, 2);
  }
  else word_count(argc, argv, 1);
  return 0;
}


