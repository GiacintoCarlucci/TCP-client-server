/* TCPClient.c - code for client program that uses TCP */

#ifndef unix
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PROTOPORT 5193                                    /* default protocol port number */
#define BUFLEN 1024                                       /* buffer size for sending and receiving data */
static const char STOP_MESSAGE[] = "STOP CLIENT PROCESS"; /* stop message that terminates the client when received */

extern int errno;
char localhost[] = "localhost";   /* default host name */

/*-----------------------------------------------------------------------
Program:  client

Purpose:  allocate a socket, connect to a server, send two nubers
          and an operation character, then print server response

Syntax:   client [host [port]]

          host - name of a computer on which server is executing
          port - protocol port number server is running

Note:     Both arguments are optional. If no host name is specified,
          the client uses "localhost"; if no protocol pot is
          specified, the client uses the default given by PROTOPORT.
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
  return 1;
}

int mysend(int socket,char * string){
  // strip off trailing \n in string (substituting \n with ' '):
  string[strcspn(string,"\n")] = ' ';
  if ((send(socket,string, BUFLEN,0))== -1)
  {
       fprintf(stderr, "Failure Sending Message\n");
       close(string);
       return 0;
  }
  return 1;
}


int main (int argc, char *argv[]){

  struct hostent *ptrh;   /* pointer to a host table entry */
  struct protoent *ptrp;  /* pointer to a protocol table entry */
  struct sockaddr_in sad; /* structure to hold an IP address */
  struct msg_struct{      /* structure holding two integers and operation character */
    int a;
    int b;
    int operation;        /* ('A':Add - 'S':Subtract - 'M':Multiply - 'D':Divide) */
  };
  int sd;                 /* socket descriptor */
  int port;               /* protocol port number */
  char *host;             /* pointer to host name */
  int n;                  /* number of characters read */

  #ifdef WIN32
    WSADATA wsaData;
    WSAStartup(0x0101,&wsaData);
  #endif
    memset((char*)&sad,0,sizeof(sad));  /* clear sockaddr structure */
    sad.sin_family = AF_INET;           /* set family to Internet */

  /* Check command-line argument for protocol port and extract */
  /* port number if one is specified. Otherwise, use the default */
  /* port value given by constant PROTOPORT. */

  if (argc > 2){          /* if protocol port specified */
    port = atoi(argv[2]); /* convert to binary */
  } else {
    port = PROTOPORT;     /* use default port number */
  }
  if (port > 0){          /* test for legal value */
    sad.sin_port = htons((u_short)port);
  }else{                  /* print error message and exit */
    fprintf(stderr,"bad port number %s\n",argv[2]);
    exit(1);
  }

  /* Check host argument and assign host name. */

  if (argc > 1){
    host = argv[1];           /* if host argument specified */
  } else {
    host = localhost;
  }

  /* Convert host name to equivalent IP address and copy to sad. */

  ptrh = gethostbyname(host);
  if(((char*)ptrh) == NULL){
    fprintf(stderr,"invalid host: %s\n",host);
    exit(1);
  }else{
    struct in_addr* ina = (struct in_addr*) ptrh->h_addr_list[0];   /* resolves name address if given */
    printf("- gethostbyname( %s ): %s\n", host,inet_ntoa(*ina));
  }

  const char* ip = "127.0.0.1";                                         /*  +---------------------*/
  struct in_addr addr;                                                  /*  | gives canonical name*/
  addr.s_addr = inet_addr(ip);                                          /*  | of an IP address    */
  ptrh = gethostbyaddr((char *) &addr, 4, AF_INET);                     /*  | in this case ip is  */
  char* canonical_name = ptrh->h_name;                                  /*  | 127.0.0.1           */
  fprintf(stdout, "- gethostbyaddr( %s ): %s\n", ip, canonical_name);   /*  +---------------------*/

  memcpy(&sad.sin_addr,ptrh->h_addr,ptrh->h_length);

  /* Map TCP transport protocol name to protocol number. */

  if (((int)(ptrp = getprotobyname("tcp"))) == 0){
    fprintf(stderr,"cannot map \"tcp\" to protocol number");
    exit(1);
  }

  /* Create a socket. */

  sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
  if (sd < 0){
    fprintf(stderr,"socket creation failed\n");
    exit(1);
  }

  /* Connect the socket to the specified server. */

  if (connect(sd,(struct sockaddr*)&sad,sizeof(sad)) < 0){
    fprintf(stderr, "connect failed\n");
    exit(1);
  }

  //result string from server
  char result [200];
  //connection status
  char connection[50];
  //structure holding two numbers and an operation character
  struct msg_struct msg;

  while(1) {

    /* Get connection status from server, if connect fails, quit. */
    if( ! myreceive(sd,connection)){
     break;
    }
    printf("%s\n",connection);

    /* Gets first number */
    printf("\x1b[32m""Enter first integer: ""\x1b[0m");
    scanf("%d",&msg.a);
    /* Clear stdin from newlines */
    getchar();
    /* Gets second number */
    printf("\x1b[32m""Enter second integer: ""\x1b[0m");
    scanf("%d",&msg.b);
    /* Clear stdin from newlines */
    getchar();
    /* Gets operation character */
    printf("\x1b[32m""Enter operation character [A,S,M,D]: ""\x1b[0m");
    msg.operation = getchar();
    /* Sends struct to server */
    send(sd,&msg,sizeof(msg),0);
    /* Quit if receive fails */
    if( ! myreceive(sd,result)){
     break;
    }
    /* If server sends stop messages, terminate the client */
    if(strcmp(result,STOP_MESSAGE)==0){
      printf("\x1b[32m""Closing connection.\n""\x1b[0m");
      break;
    }
    /* Else prints result */
    printf("\x1b[34m""Message Received From Server -  %s\n""\x1b[0m",result);
    break;
  }

  /* Close the socket. */
  closesocket(sd);

  /* Terminate the program gracefully. */
  exit(0);
}
