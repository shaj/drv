#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kallsyms.h>

static int __init ksys_call_tbl_init(void) {
   void** psct = (void**)kallsyms_lookup_name("sys_call_table");
   printk("+ sys_call_table address = %px\n", psct);
   if(psct) {
      int i;
      char table[200] = "sys_call_table : ";       
      for(i = 0; i < 10; i++) 
         sprintf(table + strlen(table), "%px ", psct[i]);
      printk("+ %s ...\n", table);
   }
   return -EPERM;
}

module_init(ksys_call_tbl_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Tsiliuric <olej.tsil@gmail.com>");
MODULE_VERSION("3.4");

