#include "common.h"

char* strsock( int sockfd, int remote ) { // format socket image
   char buff[ INET_ADDRSTRLEN ];
   static char msg[ INET_ADDRSTRLEN + 6 ];
   struct sockaddr_in addr;
   socklen_t len = INET_ADDRSTRLEN;
   if( remote != 0 )
      getsockname( sockfd, (struct sockaddr*)&addr, &len );
   else
      getpeername( sockfd, (struct sockaddr*)&addr, &len );
   sprintf( msg, "%s:%d",
            inet_ntop( AF_INET, &addr.sin_addr, buff, sizeof( buff ) ),
            ntohs( addr.sin_port ) );
   return msg;
}

