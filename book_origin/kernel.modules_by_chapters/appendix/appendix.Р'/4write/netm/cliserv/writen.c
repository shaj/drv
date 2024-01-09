#include "common.h"

ssize_t writen( int fd, const void *vptr, size_t n ) {  // Write n bytes to a descriptor/socket
   size_t nleft;
   ssize_t nwritten;
   nleft = n;
   while( nleft > 0 ) {
      if( ( nwritten = write( fd, vptr, nleft ) ) <= 0 ) {
         if( errno == EINTR ) nwritten = 0;             // and call write() again 
         else return( -1 );
      }
      nleft -= nwritten;
      vptr  += nwritten;
   }
   return( n );
}
