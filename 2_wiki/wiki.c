#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main (int argc, char **argv) {

  if (argc == 1) {
    puts("Veuillez donner un nombre de pages en argument");
    return 0;
  }

  int n = atoi(argv[1]);
  int *pages = malloc(2*n * sizeof(int));
  int i;
  for (i=0; i<n; i++) if (pipe(&pages[2*i]) == -1) perror("pipe");;

  if (!fork()) {
    int i;
    for (i=0; i<n; i++) {
      dup2(pages[2*i+1], 1);
      close(pages[2*i]);
      if (!fork())
        execlp("curl", "curl", "-s", "http://www.lsv.fr/~hondet/", NULL);
    }
  }

  else {
    int i;
    for (i=0; i<n; i++) {
      dup2(pages[2*i], 0);
      close(pages[2*i+1]);
      if (!fork()) execlp("wc", "wc", "-w", NULL);
    }

    for (i=0; i<2*n; i++) wait(NULL);
    return 0;
  }

}
