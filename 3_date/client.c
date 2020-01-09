#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <ulimit.h>
#include <netdb.h>
#include <string.h>

#define MAXFILES (ulimit(4))




int connect_to_server (char *hostname, int port) {

  int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s < 0) perror("socket");

  struct hostent *host_address = gethostbyname(hostname);
  if (!host_address) perror("gethostbyname");

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  memcpy(&server_address.sin_addr,
         host_address->h_addr_list[0],
         host_address->h_length);
  server_address.sin_port = htons(port);

  if (connect(s, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
    perror("connect");;

  return s;
}





int main () {

  char msg = 'd';
  char date[26];
  int i;

  fd_set readfds;
  char *server = "localhost";
  int port = 4004;
  int socket = connect_to_server(server, port);


  for (i=0; i<10; i++) {

    sleep(1);

    FD_ZERO(&readfds);
    FD_SET(socket, &readfds);

    write(socket, &msg, 1);

    if (select(MAXFILES, &readfds, NULL, NULL, NULL) < 0) perror("select");

    if (FD_ISSET(socket, &readfds)) {
      if (read(socket, date, 26) > 0)
        printf("(%d)  D'après le serveur, la date est: %s\n", i, date);
      else {
        puts("Connexion échouée (trop de connexions sur le serveur?)");
        return 0;
      }
    }
  }

  return 0;
}
