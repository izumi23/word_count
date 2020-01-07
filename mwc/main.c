#include <stdio.h>
#include "line_count.h"
#include "word_count.h"


int char_count (char *file) {
  int f = open(file, O_RDONLY);
  return lseek(f, 0, SEEK_END);
}


int count (char opt, char *file) {
  if (opt == 'c') return char_count(file);
  else if (opt == 'l') return line_count(file);
  else return word_count(file);
}


int main (int argc, char **argv) {
  int result, i;
  int total = 0;
  int start = 1;
  char opt;
  if (argv[1][0] == '-') {
    opt = argv[1][1];
    start = 2;
  }
  for (i=start; i<argc; i++) {
    result = count(opt, argv[i]);
    printf("%d %s\n", result, argv[i]);
    total += result;
  }
  if (argc-start >= 2) printf("%d total\n", total);
  return 0;
}
