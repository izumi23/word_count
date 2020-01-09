#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <ulimit.h>
#include <time.h>
#include <fcntl.h>

#define MAXFILES (ulimit(4))




int get_server_socket (int port) {

  struct sockaddr_in server_address;

  int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s < 0) perror("socket");;

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);

  if (bind(s, (struct sockaddr*)&server_address, sizeof(server_address)))
    perror("bind");

  if (listen(s, 5)) perror("listen");

  return s;
}





int wait_for_client (int socket) {

  int client_socket = accept(socket, NULL, NULL);
  if (client_socket < 0) perror("accept");
  return client_socket;

}




int main (int argc, char **argv) {

  char c;
  char date[26];
  date[25] = '\0';

  fd_set readfds;
  int port = 4004;
  int socket = get_server_socket(port);
  int csock = wait_for_client(socket);


  while (1) {

    FD_ZERO(&readfds);
    FD_SET(csock, &readfds);

    if (select(MAXFILES, &readfds, NULL, NULL, NULL) < 0) perror("select");

    if (FD_ISSET(csock, &readfds)) {

      read(csock, &c, 1);

      int p[2];
      pipe(p);
      if (!fork()) {
        dup2(p[1], 1);
        close(p[0]);
        execlp("date", "date", "--rfc-3339=seconds", NULL);
      }

      else {
        close(p[1]);
        read(p[0], date, 25);
        close(p[0]);
        write(csock, date, 26);
      }
    }
  }

  return 0;
}
