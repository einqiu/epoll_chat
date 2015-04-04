#ifndef _LOCAL_H_
#define _LOCAL_H_

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <time.h>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
// Default buffer size
#define BUF_SIZE 1024

// Default port
#define SERVER_PORT 7000

// Default timeout
#define EPOLL_RUN_TIMEOUT -1

// Count of connections that we are planning to handle (just hint to kernel)
#define EPOLL_SIZE 10000

// First welcome message from server
#define STR_WELCOME "PLEASE INPUT YOUT NICKNAME WHICH IS YOUR UNIQUE ID:\n"

// Format of message population
#define STR_MESSAGE "%s:%s"

// Macros - exit in any error (eval < 0) case
#define CHK(eval) if(eval < 0){perror("eval"); exit(-1);}

// Macros - same as above, but save the result(res) of expression(eval) 
#define CHK2(res, eval) if((res = eval) < 0){perror("eval"); exit(-1);}

//Macros remind all the users: Login
#define STR_IN "%s JUST LOGIN\n"

//Macros remind all the users: Logout
#define STR_OUT "%s JUST LOGOUT\n"

//Macros the result of query
#define STR_QUERY "THE RESULT OF THE QUERY IS:\n%s"

// Preliminary declaration of functions
int setnonblocking(int sockfd);
void debug_epoll_event(epoll_event ev);
int handle_message(int new_fd);
int print_incoming(int fd);
//shut down the socket
void close_client( char message[],int new_fd);
#endif
