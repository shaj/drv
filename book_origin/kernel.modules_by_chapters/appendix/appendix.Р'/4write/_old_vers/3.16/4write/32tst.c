#include <linux/module.h>

#define PREFIX "! "
#define LOG(...) printk( KERN_INFO PREFIX __VA_ARGS__ )
static int __init init( void ) {
#ifdef __i386__
   LOG( "detected 32-bit platform\n" );
#else
   LOG( "detected 64-bit platform\n" );
#endif
   return -EPERM;
}
module_init( init );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej.tsil@gmail.com>" );
