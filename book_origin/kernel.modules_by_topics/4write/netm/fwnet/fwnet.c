#include <linux/module.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/inet.h>
#include <linux/netdevice.h>

#define PREFIX "! "
#define DEB(...) if( debug ) printk( KERN_INFO PREFIX __VA_ARGS__ )
#define DEB2(...) if( debug > 1 ) printk( KERN_INFO PREFIX " ---- " __VA_ARGS__ )
#define LOG(...) printk( KERN_INFO PREFIX __VA_ARGS__ )
#define ERR(...) printk( KERN_ERR PREFIX __VA_ARGS__ )

static int debug = 0;                                       // debug output level: 0, 1, 2
module_param( debug, uint, 0 );
static char* deny;                                          // string parameter: denied IPv4
module_param( deny, charp, 0 );
static int port = 0;                                        // denied port
module_param( port, int, 0 );

static void **taddr;                                        // table sys_call_table address
u32 ipdeny;                                                 // denied IP

#include "find.c"
#include "CR0.c"

inline char* in4_ntoa( uint32_t ip ) {                      // mapping IP to a string
   static char saddr[ MAX_ADDR_LEN ];
   sprintf( saddr, "%d.%d.%d.%d",
            ( ip >> 24 ) & 0xFF, ( ip >> 16 ) & 0xFF, 
            ( ip >> 8 ) & 0xFF, ( ip ) & 0xFF
          );
   return saddr;
}

// <linux/syscalls.h> :
asmlinkage long (*old_sys_socketcall) ( int call, unsigned long __user *args );

asmlinkage long new_sys_socketcall( int call, unsigned long __user *args ) {
#define PARMS 3
   /* <linux/socket.h> :
      struct sockaddr {
         sa_family_t     sa_family;      // address family, AF_xxx
         char            sa_data[14];    // 14 bytes of protocol address
   }; */
   static unsigned long a[ PARMS ];                         // accept() and connect() have the same number of parameters 3
   static struct sockaddr sa;
   // ----------- nested functions are a GCC extension ---------
   long get_addr( void ) {
      const unsigned int len = PARMS * sizeof( unsigned long );
      if( copy_from_user( a, args, len ) )
         return -EFAULT;
      if( copy_from_user( &sa, (struct sockaddr __user*)a[ 1 ], sizeof( struct sockaddr ) ) )
         return -EFAULT;
      return 0;
   }
   // ----------------------------------------------------------
   /* <linux/in.h> :
      struct sockaddr_in {
         __kernel_sa_family_t  sin_family;     // Address family
         __be16                sin_port;       // Port number
         struct in_addr        sin_addr;       // Internet address
            ... 
      };
      struct in_addr {
         __be32  s_addr;
      }; */
   /* <linux/byteorder/generic.h> :
      #define ___ntohl(x) __be32_to_cpu(x)
      #define ___ntohs(x) __be16_to_cpu(x)
      #define ntohl(x) ___ntohl(x)
      #define ntohs(x) ___ntohs(x) */
   long ret;
   if( SYS_ACCEPT == call ) {                               // accept() before syscall
      long err;
      if( ( err = get_addr() ) < 0 ) return err;
      if( AF_INET == sa.sa_family ) {                       // only IPv4
         struct sockaddr_in *usin = (struct sockaddr_in *)&sa;
         if( ntohs( usin->sin_port ) == port ) {
            LOG( "accept from denied port %d\n", ntohs( usin->sin_port ) );
            return -EIO;
         }
      }
   }
   if( SYS_CONNECT == call ) {                              // connect() before syscall
      long err;
      if( ( err = get_addr() ) < 0 ) return err;
      if( AF_INET == sa.sa_family ) {                       // only IPv4
         struct sockaddr_in *usin = (struct sockaddr_in *)&sa;
         DEB( "connect to %s:%d\n",
              in4_ntoa( ntohl( usin->sin_addr.s_addr ) ), ntohs( usin->sin_port ) );
         if( ( deny != NULL && ntohl( usin->sin_addr.s_addr ) == ipdeny ) ||
             ( port  != 0 && ntohs( usin->sin_port ) == port ) )  {
            LOG( "connect to %s:%d denied\n",
                 in4_ntoa( ntohl( usin->sin_addr.s_addr ) ), ntohs( usin->sin_port ) );
            return -EACCES;
         }
      }
   }
   ret = old_sys_socketcall( call, args );                  // retranslate to original sys_socketcall()
   if( SYS_ACCEPT == call ) {                               // accepr() after syscall
      long err;
      if( ( err = get_addr() ) < 0 ) return err;
      if( AF_INET == sa.sa_family ) {                       // only IPv4
         struct sockaddr_in *usin = (struct sockaddr_in *)&sa;
         DEB( "accept from %s:%d\n", 
              in4_ntoa( ntohl( usin->sin_addr.s_addr ) ), ntohs( usin->sin_port ) );
         if( ( deny != NULL && ntohl( usin->sin_addr.s_addr ) == ipdeny ) ||
             ( port  != 0 && ntohs( usin->sin_port ) == port ) )  {
            LOG( "accept from %s:%d denied\n",
                 in4_ntoa( ntohl( usin->sin_addr.s_addr ) ), ntohs( usin->sin_port ) );
            return -EACCES;
         }
      }
   }
   return ret;
}
EXPORT_SYMBOL( new_sys_socketcall );

static int __init init( void ) {
   void *waddr;
   // ----------- nested functions are a GCC extension ---------
   int pos_in_table( const char *symbol ) {                 // position in sys_call_table (__NR_*)
      const int last = __NR_process_vm_writev;              // near last syscall in i386
      int n;
      waddr = find_sym( symbol );
      if( NULL == waddr ) return -1;
      for( n = 0; n <= last; n++ )
         if( taddr[ n ] == waddr ) break;
      return n <= last ? n : -1;
   }
   // --------------------------------------------------------
   void show_in_table( char *symb ) {                       // print info about symbol
      waddr = find_sym( symb );
      if( NULL == waddr ) 
         DEB( "symbol %s not found in kernel\n", symb );
      else {
         int n = pos_in_table( symb );
         if( n > 0 ) 
            DEB( "symbol %s address = %p, position in sys_call_table = %d\n", symb, waddr, n );
         else
            DEB( "symbol %s address = %p, not found in sys_call_table\n", symb, waddr );
      }
   }
   // --------------------------------------------------------
   ipdeny = ntohl( deny != NULL ? in_aton( deny ) : in_aton( "0.0.0.0" ) );
   LOG( "denied IP: %s\n", deny != NULL ? in4_ntoa( ipdeny ) : "no" );
   if( port != 0 )
      LOG( "denied TCP port: %d\n", port );
   if( NULL == ( taddr = find_sym( "sys_call_table" ) ) ) {
      ERR( "sys_call_table not found\n" ); return -EINVAL;
   }
   DEB( "sys_call_table address = %p\n", taddr );
   show_in_table( "sys_accept" );
   show_in_table( "sys_connect" );
   show_in_table( "sys_socketcall" );                       // only diagnostic
   old_sys_socketcall = (void*)taddr[ __NR_socketcall ];
   if( NULL == ( waddr = find_sym( "sys_socketcall" ) ) ) { // sys_socketcall not exported
      ERR( "sys_socketcall not found\n" ); return -EINVAL;
   }
   if( old_sys_socketcall != waddr ) {                      // reinsurance!
      ERR( "Oooops! I don't understand: addresses not equal\n" ); return -EINVAL;
   }
   if( debug ) show_cr0();
   rw_enable();
   taddr[ __NR_socketcall ] = new_sys_socketcall;
   if( debug ) show_cr0();
   rw_disable();
   if( debug ) show_cr0();
   LOG( "install new sys_socketcall handler: %p\n", &new_sys_socketcall );
   return 0;
}

static void __exit exit( void ) {
   LOG( "sys_socketcall handler before unload: %p\n", (void*)taddr[ __NR_socketcall ] );
   rw_enable();
   taddr[ __NR_socketcall ] = old_sys_socketcall;
   rw_disable();
   LOG( "restore old sys_socketcall handler: %p\n", (void*)taddr[ __NR_socketcall ] );
   return;
}

module_init( init );
module_exit( exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej.tsil@gmail.com>" );
MODULE_VERSION( "1.5" );
