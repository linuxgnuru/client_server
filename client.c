/*
  John Wiggins
  5/8/96
  
  Client program for UPS battery.
  (this does not actualy recieve data, but could easily.)
  Input: every 45 seconds (1 cycle) receives boolean value
         from UPS battery; either "has power" or "no power"
  Output: after each cycle, open socket to other computers
          also plugged into the UPS battery.  Send them
          boolean value similar to that recieved from 
          the battery.  Close connection and, if recieved
          the "no power", proceed to shutdown the system.  
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h> // For seed randomizer.

#define POWER "power"
#define NOPOWER "nopower"
#define LOCALHOST "localhost"
#define PORT "1974"

int main(int argc, char **argv)
{
  /* 
    Random number: power loss or not. 
    (get two random numbers and compare them.
    if they are equal, then send NOPOWER, else
    send POWER.  This should allow more POWER
    than NOPOWER problems.) (JCW)
  */
  int randNum1=0;
  int randNum2=0;
  int sock;
  struct sockaddr_in server;
  struct hostent *hp, *gethostbyname();
  char buf[1024];

  // Randomize seed. (JCW)
  srandom(time(NULL));
  // Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("opening stream socket");
    exit(1);
  }
  // Connect socket using name specified by command line.
  server.sin_family = AF_INET;
  // (JCW) Force ip to be local host; loopback.  NOTE: This is only for this debuging version.
  hp = gethostbyname(LOCALHOST);
  if (hp == 0)
  {
    fprintf(stderr, "%s: unknown host\n", LOCALHOST);
    exit(2);
  }
  bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
  // (JCW) Force port address.  (not really sure if this will work.)
  server.sin_port = htons(atoi(PORT));
  if (connect(sock, &server, sizeof(server)) < 0)
  {
    perror("connecting stream socket");
    exit(1);
  }
  // Make two random numbers from 1 to 10. (JCW)
  randNum1 = (rand() % 10) + 1;
  randNum2 = (rand() % 10) + 1;
  // Power lost. (JCW)
  if (randNum1 == randNum2)
  {
    printf("no power\n");
    if (write(sock, NOPOWER, sizeof(NOPOWER)) < 0)
    {
      perror("writing on stream socket");
    }
  }
  // Power isn't lost. (JCW)
  else
  {
    printf("power\n");
    if (write(sock, POWER, sizeof(POWER)) < 0)
    {
      perror("writing on stream socket");
    }
  }
  close(sock);
  return EXIT_SUCCESS;
}

