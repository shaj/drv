//#include "../common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>  
#include <linux/netdevice.h>
#include <time.h>
#include <sys/timeb.h>

#include <sys/socket.h>	                   // basic socket definitions
#include <arpa/inet.h>

// Definitions for TCP client/server programs:
#define SERV_TCP_PORT   60000
#define      SERV_HOST_ADDR  "127.0.0.1"   // default host addr for server
#define LEN_MSG 256                        // text messages max length
#define ERR(...)  fflush( stdout ), fprintf( stderr, __VA_ARGS__ ), fflush( stderr ), exit( EXIT_FAILURE )

ssize_t writen( int fd, const void *vptr, size_t n );
ssize_t readline( int fd, void *vptr, size_t maxlen );
char* strsock( int sockfd, int remote );
