static void* find_sym(const char *sym) {  // find address kernel symbol
   static uintptr_t faddr = 0;       // static !!!
   // ----------- nested functions are a GCC extension ---------
   int symb_fn(void* data, const char* sym, struct module* mod, uintptr_t addr) {
      if(0 == strcmp( (char*)data, sym)) {
         faddr = addr;
         return 1;
      }
      else return 0;
   };
   // --------------------------------------------------------
   kallsyms_on_each_symbol(symb_fn, (void*)sym);
   return (void*)faddr;
}
