/* TCPServer.c - code for example server program that uses TCP */
#ifndef unix
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>      /* needed for tolower and toupper functions */

#define PROTOPORT 5193  /* default protocol port number */
#define QLEN 6          /* size of request queue */
#define BUFLEN 1024     /* buffer size for sending and receiving data */

int visits = 0;         /* counts client connections */

/*-----------------------------------------------------------------------
Program:  server
Purpose:  allocate a socket and thenn repeatedly execute the following:
          (1) wait for the next connection from a client
          (2) send a short message to the client
          (3) close the connection
          (4) go back to step (1)

Syntax:   TCPServer [ port ]
          port - protocol port number to use

Note:     The port argument is optional. If no port is specified,
          the server uses the default given by PROTOPORT.

-----------------------------------------------------------------------*/

int myreceive(int socket,char *string){
  int num = 0;
  if ((num = recv(socket, string, BUFLEN,0)) == -1) {
          perror("Error receiving data.");
          exit(1);
  } else if (num == 0) {
          printf("Connection closed\n");
          return 0;
  }
  string[num] = '\0';
  printf("\x1b[33m""received: %s\n""\x1b[0m",string );
  return 1;
}

int mysend(int socket,char *string){
  if ((send(socket,string, BUFLEN,0))== -1)
  {
       fprintf(stderr, "Failure Sending Message\n");
       close(string);
       return 0;
  }
  printf("sent: %s\n",string);
  return 1;
}

int main(int argc,char *argv[]){

  struct hostent *ptrh;   /* pointer to a host table entry */
  struct protoent *ptrp;  /* pointer to a protocol table entry */
  struct sockaddr_in sad; /* structure to hold server's address */
  struct sockaddr_in cad; /* structure to hold client's address */
  int sd, sd2;            /* socket descriptors */
  int port;               /* protocol port number */
  int alen;               /* address lenght */
  char buf[1000];         /* string messages server buffer */

  #ifdef WIN32
    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);
  #endif
  memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
  sad.sin_family = AF_INET;           /* set family to Iternet    */
  sad.sin_addr.s_addr = INADDR_ANY;   /* set the local IP address */

  /* Check command-line argument for protocol port and extract    */
  /* port number if one is specified. Otherwise, use the default  */
  /* port value given by constant PROTOPORT                       */

  if (argc > 1) {          /* if argument specified               */
    port = atoi(argv[1]);  /* convert argument to binary          */
  } else {
    port = PROTOPORT;      /* use default port number             */
  }
  if (port > 0) {          /* test for illegal value              */
    sad.sin_port = htons((u_short)port);
  } else {                 /* print error message and exit        */
    fprintf(stderr,"bad port number %s\n",argv[1]);
    exit(1);
  }

  /* MAp TCP transport protocol name to protocol number           */

  if (((int)(ptrp = getprotobyname("tcp"))) == 0) {
    fprintf(stderr, "cannot map \"tcp\" to protocol number");
    exit(1);
  }

  /* Creat a socket                                               */

  sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
  if (sd < 0){
    fprintf(stderr, "socket creation failed\n");
    exit(1);
  }

  /* Bind a local address to the socket                           */

  if (bind(sd, (struct sockaddr *)&sad,sizeof(sad)) < 0){
    fprintf(stderr, "bind failed\n");
    exit(1);
  }

  /* Specify size of request queue                                */

  if (listen(sd, QLEN) < 0){
    fprintf(stderr,"listen failed\n");
    exit(1);
  }

  /* Main server loop - accept and handle requests                */
  while (1){
    alen = sizeof(cad);
    if ((sd2=accept(sd,(struct sockaddr *)&cad,&alen)) < 0){
      fprintf(stderr, "accept failed\n");
      exit(1);
    }

    char string_1[50];
    char string_2[50];
    char connected_succ[50];
    //creating result string
    char result_string [100];

    snprintf(connected_succ,sizeof(connected_succ),"Connessione avvenuta");
    mysend(sd2,connected_succ);
        while(1) {
          myreceive(sd2,string_1);
          myreceive(sd2,string_2);
          for(int i = 0; string_1[i]; i++){
            string_1[i] = tolower(string_1[i]);
          }
          for(int i = 0; string_2[i]; i++){
            string_2[i] = toupper(string_2[i]);
          }
          snprintf(result_string, sizeof(result_string), "%s %s", string_1, string_2);
          mysend(sd2,result_string);
          break;
        } //End of Inner While...
        //Close Connection Socket
        closesocket(sd2);
  }//End of outer while
  closesocket(sd2);
  exit(0);

}
