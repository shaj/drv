#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include "find.c"

/* <linux/syscalls.h>
asmlinkage long sys_write( unsigned int fd, 
                           const char __user *buf,
                           size_t count ); */

static asmlinkage long (*sys_write) (
   unsigned int, const char __user *, size_t );

static int __init wr_init(void) {
   char buf[80] = "Hello from kernel!\n";
   int len = strlen(buf), n;

#if defined(CONFIG_X86_64)
   sys_write = find_sym("__x64_sys_write");
#elif defined(CONFIG_X86_32)
   sys_write = find_sym("__ia32_sys_write");
#else 
   printk("+ only for x86 architecture!\n");
   return -ENOENT;
#endif
   printk("+ sys_write address = %px\n", sys_write);
   if(sys_write != NULL) {
      mm_segment_t old_fs;   
      old_fs = get_fs();                 // буфер в kernel space
      set_fs(KERNEL_DS);   
      printk("+ [%px:%d]: %s\n", &buf, len, buf);
//      mm_segment_t fs = get_fs();
//      set_fs( get_ds() );
//      n = sys_write(1, buf, len);
      set_fs(old_fs);      
//      set_fs( fs );
      printk("+ printf() return : %d\n", n);
      return -EPERM; 
   }
   else {
      printk("+ sys_write not found!\n");
      return -ENOENT;
   }
}

module_init(wr_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Tsiliuric <o.tsiliuric@yandex.ua>");
