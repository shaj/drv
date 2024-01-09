#include	"common.h"

static int debug_level = 0;                // output level

void str_echo( int sockfd ) {
/* Read a stream socket one line at a time, and write each line back to the sender.
 * Return when the connection is terminated. */
   char recvline[ LEN_MSG ];               // reaad buffer
   int n;
   for( ; ; ) {
      n = readline( sockfd, recvline, LEN_MSG );
      if( n == 0 ) break;                  // connection terminated, EOF
      else if( n < 0 ) ERR( "readline error: %m\n" );
      if( debug_level > 1 ) fprintf( stdout, "echo %d bytes: %s", n, recvline );
      if( writen( sockfd, recvline, n ) != n ) ERR( "writen error: %m\n" );
   }
}

int main( int argc, char* argv[] ) {
   char sopt[] = "p:v";
   int c, port = SERV_TCP_PORT;
   while( -1 != ( c = getopt( argc, argv, sopt ) ) )
      switch( c ) {
         case 'p':                         // listening port for TCP
            if( atoi( optarg ) > 0 ) port = atoi( optarg );
            break;
         case 'v':
            debug_level++;
            break;
         default :
            ERR( "use: %s [-p<TCP port>] [-v[v]]\n", argv[ 0 ] );
      }
   if( debug_level > 0 )
      fprintf( stdout, "listening on the TCP port %d\n", port );
   int sockfd;                             // open a TCP stream socket
   if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
      ERR( "can't open stream socket: %m\n" );
   struct sockaddr_in serv_addr;           // bind our local address 
   bzero( (char*)&serv_addr, sizeof( serv_addr ) );
   serv_addr.sin_family      = AF_INET;
   serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
   serv_addr.sin_port        = htons( port );
   if( bind( sockfd, (struct sockaddr*)&serv_addr, sizeof( serv_addr ) ) < 0 )
      ERR( "can't bind local address: %m\n" );
   listen( sockfd, 5 );
   for ( ; ; ) {
      unsigned int clilen = sizeof( serv_addr );
      int pid,
          newsockfd = accept( sockfd, (struct sockaddr*)&serv_addr, &clilen );
      if( newsockfd < 0 ) {
         if( EIO == errno )
            ERR( "denied TCP port: %m\n" );
         if( EACCES == errno ) {
            fprintf( stdout, "connect from denied IP: %m\n" );
            usleep( 500000 );              // pause 0.5 sec. before repeating
            continue;
         }
         ERR( "accept error: %m\n" );
      }
      if( ( pid = fork() ) < 0 ) ERR( "fork error: %m\n" );
      else if( pid == 0 ) {                // child process
         close( sockfd );                  // close original socket
         if( debug_level > 0 ) {
            fprintf( stdout, "connect from: %s", strsock( newsockfd, 1 ) );
            fprintf( stdout, " <= %s\n", strsock( newsockfd, 0 ) );
         }
         str_echo( newsockfd );            // process the request
         if( debug_level > 0 ) fprintf( stdout, "connection closed\n" );
         exit( 0 );
      }
      close( newsockfd );                  // parent process
   }
}
