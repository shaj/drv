#include "umd1.h"

static int __init init(void) {
   int i;
   for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
      printk("%s\n", tests[i]());
   return -1;
}
