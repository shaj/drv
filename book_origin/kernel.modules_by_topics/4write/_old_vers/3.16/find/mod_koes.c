#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include "find.c"

static int __init ksys_call_tbl_init( void ) {
   void **sct = find_sym( "sys_call_table" );   // table sys_call_table address
   printk( "+ sys_call_table address = %p\n", sct );
   if( sct != NULL ) {
      int i;
      char table[ 120 ] = "sys_call_table : ";
      for( i = 0; i < 10; i++ )
         sprintf( table + strlen( table ), "%p ", sct[ i ] );
      printk( "+ %s ...\n", table );
   }
   return -EPERM;
}

module_init( ksys_call_tbl_init );
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
