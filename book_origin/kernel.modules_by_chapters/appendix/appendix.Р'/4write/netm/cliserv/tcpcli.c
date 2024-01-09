#include	"common.h"

static void str_cli( register FILE* fp, register int sockfd ) {
/* Read the contents of the FILE *fp (stream), write each line to the
 * stream socket (to the server process), then read a line back from
 * the socket and write it to the standard output.
 * Return to caller when an EOF is encountered on the input file. */
   char sendline[ LEN_MSG ],                       // write buffer
        recvline[ LEN_MSG + 1 ];                   // reaad buffer
   int n;
   while( fgets( sendline, LEN_MSG, fp ) != NULL ) {
      n = strlen( sendline );
      if( writen( sockfd, sendline, n ) != n ) ERR( "writen error on socket: %m\n" );
      n = readline( sockfd, recvline, LEN_MSG );   // read a line from the socket and write it to our standard output
      if( n < 0 ) ERR( "readline error: %m\n" );
      recvline[ n ] = 0;                           // null terminate
      fputs( recvline, stdout );
   }
   if( ferror( fp ) )
      ERR( "error reading file: %m\n" );
}

int main( int argc, char* argv[] ) {
   char sopt[] = "h:p:v", host_ip[ 17 ] = SERV_HOST_ADDR;
   int c, port = SERV_TCP_PORT,
       debug_level = 0;                            // debug output level
   while( -1 != ( c = getopt( argc, argv, sopt ) ) )
      switch( c ) {
         case 'h':                                 // server IP address
            strncpy( host_ip, optarg, sizeof( host_ip ) );
            break;
         case 'p':                                 // listening port for TCP
            if( atoi( optarg ) > 0 ) port = atoi( optarg );
            break;
         case 'v':
            debug_level++;
            break;
         default :
            ERR( "use: %s [-h<IP address>] [-p<TCP port>] [-v]\n", argv[ 0 ] );
      }
   int sockfd;
   struct sockaddr_in serv_addr;
   bzero( (char*)&serv_addr, sizeof( serv_addr ) );
   serv_addr.sin_family      = AF_INET;            // fill in the structure address of the server
   serv_addr.sin_addr.s_addr = inet_addr( host_ip );
   serv_addr.sin_port        = htons( port );
   if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
      ERR( "can't open stream socket: %m\n" ); // open a TCP socket 
   if( connect( sockfd, (struct sockaddr*)&serv_addr, sizeof( serv_addr ) ) < 0 )
      ERR( "can't connect to server: %m\n" );
   if( debug_level > 0 ) {
      fprintf( stdout, "connect to: %s", strsock( sockfd, 1 ) );
      fprintf( stdout, " => %s\n", strsock( sockfd, 0 ) );
   }
   str_cli( stdin, sockfd );                        // do it all
   close( sockfd );
   exit( 0 );
}
