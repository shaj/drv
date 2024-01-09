#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/unistd.h> 

// static inline int kallsyms_on_each_symbol(
//      int (*fn)(void *, const char *, struct module *, unsigned long),
//      void *data) 

static void* find_sym(const char *sym) {
   static unsigned long faddr = 0; // static!!!
   // ----------- вложенная функция - расширение GCC ---------
   int symb_fn(void* data, const char* sym, struct module* mod, unsigned long addr) {
      if(0 == strcmp((char*)data, sym)) {
         faddr = addr;
         return 1;
      }
      else return 0;
   }
   // --------------------------------------------------------
   faddr = 0;
   kallsyms_on_each_symbol(symb_fn, (void*)sym);
   return (void*)faddr;
}
