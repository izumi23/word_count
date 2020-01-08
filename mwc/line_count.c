#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>



typedef struct {
  char *file;
  int start;
  int end;
} line_file_section;



int line_total = 0;
pthread_mutex_t line_mutex = PTHREAD_MUTEX_INITIALIZER;



void *line_count_thread (void *args) {
  int i;
  char c;
  int count = 0;
  line_file_section *s = args;
  int f = open(s->file, O_RDONLY);
  if (f == -1) perror("open");
  lseek(f, s->start, SEEK_SET);
  for (i = s->start; i < s->end; i++) {
    if (read(f, &c, 1) == -1) perror("read");
    if (c == '\n') count++;
  }
  pthread_mutex_lock(&line_mutex);
  line_total += count;
  pthread_mutex_unlock(&line_mutex);
  pthread_exit(NULL);
}



int line_count (char *file) {
  int f, size, frac_size, i;
  pthread_t t[4];
  f = open(file, O_RDONLY);
  if (f == -1) perror("open");
  size = lseek(f, 0, SEEK_END);
  if (size == -1) perror("lseek");
  frac_size = size/4;

  for (i=0; i<4; i++) {
    int res;
    line_file_section *s = malloc(sizeof(line_file_section));
    s->file = file;
    s->start = i*frac_size;
    s->end = i==3? size: (i+1)*frac_size;
    pthread_create(&t[i], NULL, line_count_thread, s);
  }

  for (i=0; i<4; i++) pthread_join(t[i], NULL);
  return line_total;
}
