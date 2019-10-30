# TCP-Client-Server
## Table of contents
* [General info](#general-info)
* [Technologies](#technologies)
* [Setup](#setup)
* [Launch](#launch)
* [Example of use](#example-of-use)

## General info
This project is a simple TCP Client-Server application divided in two main actors:

**SERVER**  
allocate a socket and then repeatedly execute the following:  
- (1) wait for the next connection from a client
- (2) send a connection confirmation to the client
- (3) receive two numbers and an operation character from the client
- (4) calulate the operation between the two numbers
- (5) send result to client
- (6) go back to step (1)
  
 **CLIENT**  
allocate a socket, then:
- (1) receive from stdin two numbers and an operation character
- (2) send the numbers and the operation character to the server
- (3) receive from server the result and prints it
- (4) close the program

## Technologies
Project is created with:
- C

Using:  
```
- <windows.h>------//Declaration of functions, macros and data types used in windows
- <winsock.h>------//Windows socket handling
- <sys/types.h>----//Defines a collection of typedef symbols and structures
- <sys/socket.h>---//Declarations of socket constants, types, and functions
- <netinet/in.h>---//Definitions for the internet protocol family
- <arpa/inet.h>----//Definitions for internet operations
- <netdb.h>--------//Definitions for network database operations
- <stdlib.h>-------//Standard general library
- <stdio.h>--------//Standard I/O library
- <string.h>-------//Definitions functions for manipulating arrays of characters
- <ctype.h>--------//Characther handling, used only for toupper() function
```
## Setup
There is no setup process, just compile the .c files.  
**Example (using gcc in terminal):**  
```
gcc TCPServer -o server
```
```
gcc TCPClient -o client
```  
At this point you will have two files named **client** and **server**, these are your executable files.  
  
**[NOTE]:** If you are using Linux you will need to make these files executable:
```
chmod +x server
```
```
chmod +x client
```
## Launch
If you compiled the source code in Windows just double click on the .exe files.  
If you compiled the source code in linux just run the following in terminal:  
```
./server
```
then open another terminal and run:
```
./client
```
In order to make everything work correctly, make sure to start server first, then start client.  
The client will stop once received the result message from the server.  
The server will remain listening for new clients. To stop the server, just press **CTRL + C**.
