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
} file_section;



int total = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;



void *word_count_thread (void *args) {
  int i;
  char c;
  int count = 0;
  int sep = 0;
  file_section *s = args;
  int f = open(s->file, O_RDONLY);
  lseek(f, s->start, SEEK_SET);
  for (i = s->start; i < s->end; i++) {
    if (read(f, &c, 1) == -1) perror("read");
    if (c == ' ' || c == '\n' || c == '\t') sep = 1;
    else if (sep) {sep = 0; count++;}
  }
  pthread_mutex_lock(&mutex);
  total += count;
  pthread_mutex_unlock(&mutex);
  pthread_exit(NULL);
}



int word_count (char *file) {
  int f, size, frac_size, i;
  pthread_t t[4];
  f = open(file, O_RDONLY);
  size = lseek(f, 0, SEEK_END);
  frac_size = size/4;

  for (i=0; i<4; i++) {
    int res;
    file_section *s = malloc(sizeof(file_section));
    s->file = file;
    s->start = i*frac_size;
    s->end = i==3? size: (i+1)*frac_size;
    pthread_create(&t[i], NULL, word_count_thread, s);
  }

  for (i=0; i<4; i++) pthread_join(t[i], NULL);
  return total;
}
