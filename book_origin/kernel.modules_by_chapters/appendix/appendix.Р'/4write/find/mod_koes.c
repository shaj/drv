#include <linux/module.h>
#include <linux/kallsyms.h>
#include "find.c"

static int __init ksys_call_tbl_init(void) {
   const char* symbol = "sys_call_table";
   void** sct = (void**)find_sym(symbol);
   printk("+ %s address = %px\n", symbol, sct); 
   if(sct) {
      char table[200] = "";
      int i;
      for(i = 0; i < 10; i++) 
         sprintf(table + strlen(table), "%px ", sct[i]);
      printk("+ %s : %s ...\n", symbol, table);
   }
   else
      printk("+ %s : %s\n", symbol, "not found");
   return -EPERM;
}

module_init(ksys_call_tbl_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Tsiliuric <o.tsiliuric@yandex.ua>");
MODULE_VERSION("3.5");
