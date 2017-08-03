/* Socket-based interprocess communication */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

/**********************************************************************/
int InitSocketServer(int Port)
{
      int init_sockfd,sockfd,flags;
      socklen_t clilen;
      struct sockaddr_in Server, Client;

      init_sockfd = socket(AF_INET,SOCK_STREAM,0);
      if (init_sockfd < 0) {
         printf("Error opening server socket.\n");
         exit(1);
      }
      memset((char *) &Server,0,sizeof(Server));
      Server.sin_family = AF_INET;
      Server.sin_addr.s_addr = INADDR_ANY;
      Server.sin_port = htons(Port);
      if (bind(init_sockfd,(struct sockaddr *) &Server,sizeof(Server)) < 0) {
         printf("Error on binding server socket.\n");
         exit(1);
      }
      listen(init_sockfd,5);
      clilen = sizeof(Client);
      sockfd = accept(init_sockfd,(struct sockaddr *) &Client,&clilen);
      if (sockfd < 0) {
         printf("Error on accepting client socket.\n");
         exit(1);
      }
      printf("Server side of socket established.\n");
      close(init_sockfd);

      /* Keep read() from waiting for message to come */
      //flags = fcntl(sockfd, F_GETFL, 0);
      //fcntl(sockfd,F_SETFL, flags|O_NONBLOCK);

      return(sockfd);
}
/**********************************************************************/
int InitSocketClient(const char *hostname, int Port)
{
      int sockfd,flags;
      struct sockaddr_in Server;
      struct hostent *Host;

      sockfd = socket(AF_INET,SOCK_STREAM,0);
      if (sockfd < 0) {
         printf("Error opening client socket.\n");
         exit(1);
      }
      Host = gethostbyname(hostname);
      if (Host == NULL) {
         printf("Server not found by client socket.\n");
         exit(1);
      }
      memset((char *) &Server,0,sizeof(Server));
      Server.sin_family = AF_INET;
      memcpy((char *)Host->h_addr,
         (char *)&Server.sin_addr.s_addr,
         Host->h_length);
      Server.sin_port = htons(Port);
      if (connect(sockfd,(struct sockaddr *) &Server,sizeof(Server)) < 0) {
         printf("Error connecting client socket: %s.\n",strerror(errno));
         exit(1);
      }
      printf("Client side of socket established.\n");

      /* Keep read() from waiting for message to come */
      flags = fcntl(sockfd, F_GETFL, 0);
      fcntl(sockfd,F_SETFL, flags|O_NONBLOCK);

      return(sockfd);
}
/**********************************************************************/
int main(int argc, char **argv)
{
      int sockfd,Success;
      double SimTime,Tx,Ty,Tz;
      long Done = 0;

      char line[512];

      sockfd = InitSocketServer(4422);

      while(!Done) {
         bzero(line,512);
         Success = recv(sockfd,line,511,0);
         //if (Success < 0) {
         //   printf("Error reading from socket in FswListener.c\n");
         //   exit(1);
         //}
         //printf("%s",line);
         //if (Success > 0) {
            if (sscanf(line,"%lf %lf %lf %lf",
               &SimTime,&Tx,&Ty,&Tz) == 4) {
               printf("SimTime: %lf   Twhlcmd: [%lf %lf %lf]\n",
                  SimTime,Tx,Ty,Tz);
            }
            else if (sscanf(line,"Done")) {
               printf("Done\n");
               Done = 1;
               close(sockfd);
            }
         //}
      }

      return(0);
}
