/*
  John Wiggins
  5/8/96
  
  Server program that listens to a port for the UPS
  status and preforms functions depending on what is
  recieved.
  
  Input: boolean value of UPS status sent from computer
         attached to UPS.
  Output: message (wall) to everyone saying that 
          the power is out and that that system
          will be shutting down.  In real version,
          this will then preform an actual shutdown.
          This will not. :)

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>

#define TRUE 1
#define POWER "power"
#define NOPOWER "nopower"

main() {
 int sock, length;
 int nop; /* no op; just something to throw in to some 'if' statement. */
 struct sockaddr_in server;
 int msgsock;
 char buf[1024];
 int rval;
 fd_set ready;
 struct timeval to;

 /* Create socket */
 sock = socket(AF_INET, SOCK_STREAM, 0);
 if (sock < 0) {
  perror("opening stream socket");
  exit(1);
 }
 /* Name socket using wildcards */
 server.sin_family = AF_INET;
 server.sin_addr.s_addr = INADDR_ANY;
 server.sin_port = htons(1974);;
 if (bind(sock, &server, sizeof(server))) {
  perror("binding stream socket");
  exit(1);
 }
 /* Find out assigned port number and print it out */
 length = sizeof(server);
 if (getsockname(sock, &server, &length)) {
  perror("getting socket name");
  exit(1);
 }
 /* printf("Socket has port #%d\n", ntohs(server.sin_port)); */
 /* Start accepting connections */
 listen(sock, 5);
 do {
  FD_ZERO(&ready);
  FD_SET(sock, &ready);
  to.tv_sec = 5;
  if (select(sock + 1, &ready, 0, 0, &to) < 0) {
   perror("select");
   continue;
  }
  if (FD_ISSET(sock, &ready)) {
   msgsock = accept(sock, (struct sockaddr *)0, (int *)0);
   if (msgsock == -1)
    perror("accept");
   else {
    do {
     bzero(buf, sizeof(buf));
     if ((rval = read(msgsock, buf, 1024)) < 0)
      perror("reading stream message");
     else {
      if (rval == 0)
       nop = 5;
       /* printf("Closing socket.\n"); */
      else {
       if (strcmp(buf, NOPOWER)) {
        printf("The system has lost power!  Shutting down!\n\a");
        rval = -1; /* Quickly get out of loop. */
       } 
       else
        printf("The system has power.\n");
      }
     }
    } while (rval > 0);
    close(msgsock);
   }
  }
 } while (TRUE);
}
