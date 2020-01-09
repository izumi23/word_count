#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <ulimit.h>


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




int get_date (int sock, char *date) {

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
    return 0;
  }
}





int main (int argc, char **argv) {

  char c;
  char date[26];
  date[25] = '\0';
  int n = 2;    //number of connections
  signal(SIGPIPE, SIG_IGN);

  fd_set readfds;
  int port = 4004;
  int socket = get_server_socket(port);

  int csock[n];
  int connected[n];
  int i;
  for (i=0; i<n; i++) connected[i] = 0;

  puts("En attente d'un client...");
  csock[0] = wait_for_client(socket);
  connected[0] = 1;
  puts("Socket 0: connecté");


  while (1) {

    FD_ZERO(&readfds);
    FD_SET(socket, &readfds);
    for (i=0; i<n; i++) if (connected[i]) FD_SET(csock[i], &readfds);

    if (select(MAXFILES, &readfds, NULL, NULL, NULL) < 0) perror("select");


    if (FD_ISSET(socket, &readfds)) {
      int success = 0;
      for (i=0; i<n; i++) {
        if (!success && !connected[i]) {
          csock[i] = wait_for_client(socket);
          success = 1;
          connected[i] = 1;
          printf("Socket %d: connecté\n", i);
        }
      }
      if (!success) {
        printf(
        "Echec d'une nouvelle connexion: serveur limité à %d connexions\n", n);
        close(wait_for_client(socket));
      }
    }


    for (i=0; i<n; i++) if (connected[i]) {

      if (FD_ISSET(csock[i], &readfds)) {

        if (read(csock[i], &c, 1) == -1) perror("read");
        get_date(csock[i], date);

        if (write(csock[i], date, 26) == -1) {
          close(csock[i]);
          connected[i] = 0;
          printf("Socket %d: déconnecté\n", i);
        }
      }
    }
  }

  return 0;
}
