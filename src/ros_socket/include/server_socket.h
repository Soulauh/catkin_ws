#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

#include <netinet/in.h>   
#include <sys/types.h>    
#include <sys/socket.h>    
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>        
#include <time.h>               
#include <arpa/inet.h>
#include <unistd.h>   

#define SERVER_PORT  10000
#define LENGTH_OF_LISTEN_QUEUE 10 
#define BUFFER_SIZE 65536

socklen_t length;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
int client_socket;
int server_socket;
char buffer[BUFFER_SIZE];

void decodeImage(int n);

#endif

