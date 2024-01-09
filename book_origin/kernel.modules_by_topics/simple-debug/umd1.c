#include "umd1.h"

inline char* mesg(const char* what) {
   static char res[120];
   sprintf(res, "this string returned from %s", what);
   return res;
}

char* test_01(void) {
   // ... здесь код теста 1
   return mesg(__FUNCTION__);
}
EXPORT_SYMBOL(test_01);

char* test_02(void) {
   // ... здесь код теста 2
   return mesg(__FUNCTION__);
}
EXPORT_SYMBOL(test_02);

char* test_03(void) {
   // ... здесь код теста 3
   return mesg(__FUNCTION__);
}
EXPORT_SYMBOL(test_03);

static int __init init(void) {
   return 0;
}

static void __exit cleanup(void) {}
module_exit(cleanup);
