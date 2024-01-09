#define PREFIX "! "

#ifdef __KERNEL__                       // для модулей
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#define DEB(...) if( debug ) printk( KERN_INFO PREFIX __VA_ARGS__ )
#define DEB2(...) if( debug > 1 ) printk( KERN_INFO PREFIX " ---- " __VA_ARGS__ )
#define LOG(...) printk( KERN_INFO PREFIX __VA_ARGS__ )
#define ERR(...) printk( KERN_ERR PREFIX __VA_ARGS__ )

#else                                    // для пиложений
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LOG(...) fprintf( stdout, __VA_ARGS__ ), fflush( stdout )
#define ERR(...) fflush( stdout ), fprintf( stderr, __VA_ARGS__ ), fflush( stderr )
//, exit( EXIT_FAILURE )

#endif

// номер нового добавляемого системного вызова
#define __NR_own 223
// может быть взят любой, полученный при загрузке модуля nsys.ko
// для ядра 3.31 был получен ряд из 27 позиций:
// 017, 031, 032, 035, 044, 053, 056, 058, 098, 112,
// 127, 130, 137, 167, 169, 188, 189, 222, 223, 251,
// 273, 274, 275, 276, 285, 294, 317, 

