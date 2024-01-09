#include "syscall.h" 
#include "./CR0.c" 

// системный вызов с двумя параметрами:
asmlinkage long (*old_sys_addr) ( void );

asmlinkage long new_sys_call ( const char __user *buf, size_t count ) {
   static char buf_msg[ 80 ];
   int res = copy_from_user( buf_msg, (void*)buf, count ); 
   buf_msg[ count ] = '\0';
   LOG( "accepted %d bytes: %s\n", count, buf_msg );
   return res;
};
EXPORT_SYMBOL( new_sys_call );

static void **taddr; // адрес таблицы sys_call_table 

static int __init new_sys_init( void ) {
   void *waddr;
   if( NULL == ( taddr = (void**)kallsyms_lookup_name( "sys_call_table" ) ) ) {
      ERR( "sys_call_table not found!\n" );
      return -EFAULT;
   }
   old_sys_addr = (void*)taddr[ __NR_own ];
   if( ( waddr = (void*)kallsyms_lookup_name( "sys_ni_syscall" ) ) != NULL ) 
      LOG( "sys_ni_syscall address = %p\n", waddr );
   else {
      ERR( "sys_ni_syscall not found!\n" );
      return -EFAULT;
   } 
   if( old_sys_addr != waddr ) {
      ERR( "not free slot!\n" );
      return -EINVAL;
   }
   LOG( "old sys_call_table[%d] = %p\n", __NR_own, taddr[ __NR_own ] );
   rw_enable();
   taddr[ __NR_own ] = new_sys_call;
   rw_disable();
   LOG( "new sys_call_table[%d] = %p\n", __NR_own, taddr[ __NR_own ] );
   return 0;
}

static void __exit new_sys_exit( void ) {
   rw_enable();
   taddr[ __NR_own ] = old_sys_addr;
   rw_disable();
   LOG( "restore sys_call_table[%d] = %p\n", __NR_own, taddr[ __NR_own ] );
   return;
}

module_init( new_sys_init );
module_exit( new_sys_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
