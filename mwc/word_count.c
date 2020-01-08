#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>



typedef struct {
  char *file;
  int rank;
  int start;
  int end;
} file_section;



static int total = 0;
static int first[4];    //0 si le premier caractère est une séparation, 1 sinon
static int last[4];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;



static void *word_count_thread (void *args) {
  int i;
  char c;
  int count = 0;
  int sep = 1;
  file_section *s = args;
  int f = open(s->file, O_RDONLY);
  if (f == -1) perror("open");
  if (lseek(f, s->start, SEEK_SET) == -1) perror("lseek");

  for (i = s->start; i < s->end; i++) {
    if (read(f, &c, 1) == -1) perror("read");
    if (i == s->start) first[s->rank] = (c != ' ' && c != '\n' && c != '\t');
    if (i == s->end-1) last[s->rank] = (c != ' ' && c != '\n' && c != '\t');
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
  if (f == -1) perror("open");
  size = lseek(f, 0, SEEK_END);
  if (size == -1) perror("lseek");
  frac_size = size/4;

  for (i=0; i<4; i++) {
    int res;
    file_section *s = malloc(sizeof(file_section));
    s->file = file;
    s->rank = i;
    s->start = i*frac_size;
    s->end = i==3? size: (i+1)*frac_size;
    if (pthread_create(&t[i], NULL, word_count_thread, s))
      perror("pthread_create");
  }

  for (i=0; i<4; i++) if (pthread_join(t[i], NULL)) perror("pthread_join");
  for (i=0; i<3; i++) if (last[i] && first[i+1]) total--;
  return total;
}
