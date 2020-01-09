#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main (int argc, char **argv) {

  if (argc == 1) {
    puts("Veuillez donner un nombre de pages en argument");
    return 0;
  }

  int i, f;
  int n = atoi(argv[1]);
  int page[2];
  int words[2];
  int classification[2] = {0, 0};

  for (i = 0; i < n; i++) {

    if (pipe(words) == -1) perror("pipe");

    if ((f=fork()) == -1) perror("fork");
    else if (!f) {

      if (pipe(page) == -1) perror("pipe");

      if ((f=fork()) == -1) perror("fork");
      else if (!f) {
        dup2(page[1], 1);
        close(page[0]);
        execlp("curl", "curl", "-Ls",
               "https://en.wikipedia.org/wiki/Special:Random", NULL);
        perror("curl");
      }

      else {
        dup2(page[0], 0);
        close(page[1]);
        dup2(words[1], 1);
        close(words[0]);
        execlp("wc", "wc", "-w", NULL);
        perror("wc");
      }
    }

    else {
      char s[16];
      if (read(words[0], s, 16) == -1) perror("read");;
      int num = atoi(s);
      printf("Page %d: %d mots\n", i+1, num);
      if (num >= 2000) classification[1]++; else classification[0]++;
      wait(NULL);
    }
  }

  printf("%d pages de moins de 2000 mots, %d pages de plus de 2000 mots\n",
         classification[0], classification[1]);
  return 0;
}
