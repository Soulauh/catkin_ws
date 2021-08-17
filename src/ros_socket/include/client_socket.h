#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

#include <netinet/in.h>   
#include <sys/types.h>    
#include <sys/socket.h>    
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>        
#include <time.h>               
#include <arpa/inet.h>
#include <unistd.h>   

#define CLIENT_PORT  10000 
#define SERVER_IP_ADDRESS "172.16.60.164"

struct sockaddr_in server_addr;

int client_socket;



#endif

