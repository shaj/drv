#include <linux/kernel.h>
#include "md1.h"

static char retpref[] = "this string returned from "; 
static char res[120];

inline void mesg(const char* what) {
//int sprintf(char *buf, const char * fmt, ...);
//   sprintf(res, "this string returned from %s", what);
//   char retpref[] = "this string returned from ",
//   strcpy(res, retpref);
//   strcat(res, what);
}

char* test_01(void) {
//   static char res[80];
   strcpy(res, retpref);
   strcat(res, __FUNCTION__);
   return res;
}
EXPORT_SYMBOL(test_01);

char* test_02(void) {
//   static char res[80];
   strcpy(res, retpref);
   strcat(res, __FUNCTION__);
//   mesg(__FUNCTION__);
   return res;
}
EXPORT_SYMBOL(test_02);

static int __init init(void) {
   return 0;
}

static void __exit cleanup(void) {}
module_exit(cleanup);

