/*#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h> */
#include "syscall.h"

static void **taddr,                     // адрес sys_call_table
            *niaddr;                     // адрес sys_ni_syscall()
static int nsys = 0;                     // число систеных вызовов в версии

#define SYS_NR_MAX 450
// SYS_NR_MAX - произвольно большое, больше длины sys_call_table
static int sys_length( void* data, const char* sym, struct module* mod, unsigned long addr ) {
   int i;
   if( ( strstr( sym, "sys_" ) != sym ) ||
       ( 0 == strcmp( "sys_call_table", sym ) ) ) return 0;
   for( i = 0; i < SYS_NR_MAX; i++ ) {
      if( taddr[ i ] == (void*)addr ) {  // найден sys_* в sys_call_table
         if( i > nsys ) nsys = i;
         break;
      }
   } 
   return 0;
}

static void put_entries( void ) {
   int i, ni = 0;
   char buf[ 200 ] = "";
   for( i = 0; i <= nsys; i++ ) 
      if( taddr[ i ] == niaddr ) {
         ni++;
         sprintf( buf + strlen( buf ), "%03d, ", i );
      } 
   LOG( "found %d unused entries: %s\n", ni, buf );
}

static int __init init_driver( void ) {
   if( NULL == ( taddr = (void**)kallsyms_lookup_name( "sys_call_table" ) ) ) {
      ERR( "sys_call_table not found!\n" ); 
      return -EFAULT;
   }
   LOG( "sys_call_table address = %p\n", taddr );
   if( NULL == ( niaddr = (void*)kallsyms_lookup_name( "sys_ni_syscall" ) ) ) {
      ERR( "sys_ni_syscall found!\n" ); 
      return -EFAULT;
   }
   LOG( "sys_ni_syscall address = %p\n", niaddr );
   kallsyms_on_each_symbol( sys_length, NULL ); 
   LOG( "sys_call_table length = %d\n", nsys + 1 );
   put_entries();
   return -EPERM;
}

module_init( init_driver );

MODULE_DESCRIPTION( "test unused entries" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_LICENSE( "GPL" );
