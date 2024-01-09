#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>

#include "./CR0.c"
#include "./find.c"

#define PREFIX "! "
#define DEB2(...) if( debug > 1 ) printk( KERN_INFO PREFIX " ---- " __VA_ARGS__ )
#define LOG(...) printk( KERN_INFO PREFIX __VA_ARGS__ )
#define ERR(...) printk( KERN_ERR PREFIX __VA_ARGS__ )

static int debug = 0;                    // debug output level: 0, 1, 2
module_param( debug, uint, 0 );

asmlinkage long (*old_sys_write) ( unsigned int fd, const char __user *buf, size_t count );

#define LEN 250
asmlinkage long new_sys_write ( unsigned int fd, const char __user *buf, size_t count ) {
   if( 1 == fd ) {
      char msg[ LEN + 1 ];
      int n = count < LEN ? count : LEN, r;
      if( ( r = copy_from_user( msg, (void*)buf, n ) ) != 0 ) return -EINVAL;
      if( '\n' == msg[ n - 1 ] ) msg[ n - 1 ] = '\0';
      else msg[ n ] = '\0';
      if( strchr( msg, '!' ) != NULL ) goto rec; // to prevent recursion
      LOG( "{%04d} %s\n", count, msg ); 
   }
rec:
   return old_sys_write( fd, buf, count );       // original write()
};
EXPORT_SYMBOL( new_sys_write );

static void **taddr;                             // address of sys_call_table 

static int __init wrchg_init( void ) {
   void *waddr; 
   if( NULL == ( taddr = find_sym( "sys_call_table" ) ) ) {
      ERR( "sys_call_table not found\n" ); return -EINVAL;
   }
   old_sys_write = (void*)taddr[ __NR_write ];
   if( NULL == ( waddr = find_sym( "sys_write" ) ) ) {
      ERR( "sys_write not found\n" ); return -EINVAL;
   }
   if( old_sys_write != waddr ) {
      ERR( "Oooops! : addresses not equal\n" ); return -EINVAL;
   }
   LOG( "set new sys_write syscall [%p]\n", &new_sys_write );
   show_cr0();
   rw_enable();
   taddr[ __NR_write ] = new_sys_write;
   show_cr0();
   rw_disable();
   show_cr0();
   return 0;
}

static void __exit wrchg_exit( void ) {
   rw_enable();
   taddr[ __NR_write ] = old_sys_write;
   rw_disable();
   LOG( "restore old sys_write syscall [%p]\n", (void*)taddr[ __NR_write ] );
   return;
}

module_init( wrchg_init );
module_exit( wrchg_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej.tsil@gmail.com>" );
