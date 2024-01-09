#include "syscall.h"

static void do_own_call( char *str ) {
   int n = syscall( __NR_own, str, strlen( str ) );
   if( n == 0 ) LOG( "syscall return %d\n", n );
   else {
      ERR( "syscall error %d : %s\n", n, strerror( -n ) ); 
      exit( n );
   }
}

int main( int argc, char *argv[] ) {
   char str[] = "DEFAULT STRING";
   if( 1 == argc ) do_own_call( str );
   else
      while( --argc > 0 ) do_own_call( argv[ argc ] );
   return EXIT_SUCCESS;
};

