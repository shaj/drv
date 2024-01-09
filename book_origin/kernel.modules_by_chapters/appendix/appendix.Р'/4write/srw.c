#include <linux/module.h>
#include <linux/init.h>
#include <linux/module.h> 
#include <linux/moduleparam.h> 
#include <linux/kallsyms.h>

#include "rw_cr0.c"
#include "rw_pte.c"
#include "rw_pax.c"
#include "rw_map.c"
#include "rw_pai.c"

#define PREFIX "! "
#define LOG(...) printk(KERN_INFO PREFIX __VA_ARGS__)
#define ERR(...) printk(KERN_ERR PREFIX __VA_ARGS__)

#ifdef __i386__                   // любая не используемая позиция sys_call_table   
#define __NR_rw_test 109  //__NR_olduname 
#else
#define __NR_rw_test 214  // __NR_epoll_ctl_old 
#endif

static int mode = 0;              
module_param(mode, uint, 0);

#define do_write(addr, val) {           \
   LOG("writing address %px\n", addr);  \
   *addr = val;                        \
}

static bool write(void** addr, void* val) {
   switch(mode) {
      case 0:   // !! ~~~~~
         rw_enable();
         do_write(addr, val);
         rw_disable();
         return true;
      case 1: 
         native_pax_open_kernel();
         do_write(addr, val);
         native_pax_close_kernel();
         return true;
      case 2: 
         mem_setrw(addr);
         do_write(addr, val);
         mem_setro(addr);
         return true;
      case 3: 
         addr = map_writable((void*)addr, sizeof(val));
         if(NULL == addr) { 
            ERR("wrong mapping\n");
            return false; 
         }
         do_write(addr, val);
         unmap_writable(addr);
         return true;
      case 4: 
         native_pai_open_kernel();
         do_write(addr, val);
         native_pai_close_kernel();
         return true;
      default:
         ERR("illegal mode %d\n", mode);
         return false;
   }
}

static int __init rw_init(void) {
   void **taddr;                                // адрес sys_call_table 
   asmlinkage long (*sys_ni_syscall) (void);    // оригинальный вызов __NR_rw_test
#ifdef __i386__
   LOG("detected 32-bit platform\n");
#else
   LOG("detected 64-bit platform\n");
#endif
   if(NULL == (taddr = (void**)kallsyms_lookup_name("sys_call_table"))) {
      ERR("sys_call_table not found\n"); return -EFAULT;
   }

   LOG("sys_call_table address = %px\n", taddr);
   sys_ni_syscall = (void*)taddr[__NR_rw_test]; // сохранить оригинал
   LOG("original sys_call_table address = %px\n", sys_ni_syscall);
   
   if(!write(taddr + __NR_rw_test, (void*)0x12345)) return -EINVAL;
   LOG("modified sys_call_table[%d] = %px\n", __NR_rw_test, taddr[ __NR_rw_test ]);
   if(!write(taddr + __NR_rw_test, (void*)sys_ni_syscall)) return -EINVAL;
   LOG("restored sys_call_table[%d] = %px\n", __NR_rw_test, taddr[ __NR_rw_test ]);
   return -EPERM;
}

module_init(rw_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Tsiliuric <olej.tsil@gmail.com>");
