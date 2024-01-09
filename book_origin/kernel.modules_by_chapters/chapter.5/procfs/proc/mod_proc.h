#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/uaccess.h>
#include "common.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Tsiliuric <olej.tsil@gmail.com>");

static int  __init proc_init(void);
static void __exit proc_exit(void);

module_init(proc_init);
module_exit(proc_exit);
