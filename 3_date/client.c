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

  fd_set readfds;
  char msg = 'd';
  char date[256];
  char *server = "localhost";
  int port = 4004;
  int socket = connect_to_server(server,port);
  int i;

  for (i=0; i<50; i++) {

    FD_ZERO(&readfds);
    FD_SET(socket, &readfds);

    write(socket, &msg, 1);

    if (select(MAXFILES, &readfds, NULL, NULL, NULL) < 0) perror("select");

    if (FD_ISSET(socket, &readfds))
      {
        read(socket, date, 256);
        printf("server said: %s\n", date);
      }
  }

  return 0;
}
