#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main (int argc, char **argv) {
  
  int n = atoi(argv[1]);
  int file_pipe[2];
  int write_pipe[2];
  int read_pipe[2];
  pipe(file_pipe);
  pipe(write_pipe);
  pipe(read_pipe);
  
  if (!fork()) {
    int i;
    char c = 'a';
    close(file_pipe[0]);
    close(write_pipe[0]);
    close(read_pipe[1]);
    dup2(file_pipe[1], 1);
    for (i=0; i<n; i++) {
      if (fork()) {
	if (i>0) write(write_pipe[1], &c, 1);
	read(read_pipe[0], &c, 1);
	execlp("curl", "curl", "-s", "http://www.lsv.fr/~hondet/", NULL);
      }
    }
    write(write_pipe[1], &c, 1);
  }
  
  else {
    int i;
    char c;
    close(file_pipe[1]);
    close(read_pipe[0]);
    close(write_pipe[1]);
    dup2(file_pipe[0], 0);
    for (i=0; i<n; i++) {
      if (fork()) {
	write(read_pipe[1], &c, 1);
	read(write_pipe[0], &c, 1);
        execlp("wc", "wc", "-w", NULL);
      }
    }
    wait(NULL);
    return 0;
  }

}
