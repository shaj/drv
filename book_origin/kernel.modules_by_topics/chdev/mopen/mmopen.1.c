#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include "mopen.h"

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Oleg Tsiliuric <olej@front.ru>" );
MODULE_VERSION( "6.4" );

static int mode = 0; // открытие: 0 - без контроля, 1 - единичное, 2 - множественное
module_param( mode, int, S_IRUGO );
static int debug = 0;
module_param( debug, int, S_IRUGO );

#define LOG(...) if( debug !=0 ) printk( KERN_INFO "! "__VA_ARGS__ )

static int dev_open = 0;

struct mopen_data {         // область данных драйвера:
   char buf[ LEN_MSG + 1 ]; // буфер данных
   int odd;                 // признак начала чтения
};

static int mopen_open( struct inode *n, struct file *f ) {
   LOG( "open - node: %p, file: %p, refcount: %ld", n, f, (long)module_refcount( THIS_MODULE ) );
   if( dev_open ) {
      LOG( "device /dev/%s is busy", DEVNAM );
      return -EBUSY;
   }
   if( 1 == mode ) dev_open++;
   if( 2 == mode ) {
      struct mopen_data *data;
      f->private_data = kmalloc( sizeof( struct mopen_data ), GFP_KERNEL );
      if( NULL == f->private_data ) {
         LOG( "memory allocation error" );
         return -ENOMEM;
      }
      data = (struct mopen_data*)f->private_data;
      strcpy( data->buf, "dynamic: not initialized!" );  // динамический буфер
      data->odd = 0;
   }
   return 0;
}
      
static int mopen_release( struct inode *n, struct file *f ) {
   LOG( "close - node: %p, file: %p, refcount: %ld", n, f, module_refcount( THIS_MODULE ) );
   if( 1 == mode ) dev_open--;
   if( 2 == mode ) kfree( f->private_data );
   return 0;
}

static struct mopen_data* get_buffer( struct file *f ) {
   static struct mopen_data static_buf = { "static: not initialized!", 0 }; // статический буфер
   return 2 == mode ? (struct mopen_data*)f->private_data : &static_buf;
}

// чтение из /dev/mopen :
static ssize_t mopen_read( struct file *f, char *buf, size_t count, loff_t *pos ) {
   struct mopen_data* data = get_buffer( f ); 
   LOG( "read - file: %p, read from %p bytes %ld; refcount: %ld",
        f, data, (long)count, module_refcount( THIS_MODULE ) );
   if( 0 == data->odd ) {
      int res = copy_to_user( (void*)buf, data->buf, strlen( data->buf ) );
      data->odd = 1;
      put_user( '\n', buf + strlen( data->buf ) );
      res = strlen( data->buf ) + 1;
      LOG( "return bytes :  %d", res );
      return res;
   }
   data->odd = 0;
   LOG( "return : EOF" );
   return 0;
}

// запись в /dev/mopen :
static ssize_t mopen_write( struct file *f, const char *buf, size_t count, loff_t *pos ) {
   int res, len = count < LEN_MSG ? count : LEN_MSG;
   struct mopen_data* data = get_buffer( f ); 
   LOG( "write - file: %p, write to %p bytes %ld; refcount: %ld",
        f, data, (long)count, module_refcount( THIS_MODULE ) );
   res = copy_from_user( data->buf, (void*)buf, len );
   if( '\n' == data->buf[ len -1 ] ) data->buf[ len -1 ] = '\0';
   else data->buf[ len ] = '\0';
   LOG( "put bytes : %d", len );
   return len;
}

static const struct file_operations mopen_fops = {
   .owner  = THIS_MODULE,
   .open =    mopen_open,
   .release = mopen_release,
   .read   =  mopen_read,
   .write  =  mopen_write,
};

static struct miscdevice mopen_dev = {
   MISC_DYNAMIC_MINOR, DEVNAM, &mopen_fops
};

static int __init mopen_init( void ) {
   int ret = misc_register( &mopen_dev );
   if( ret ) { LOG( "unable to register %s misc device", DEVNAM ); }
   else { LOG( "installed device /dev/%s in mode %d", DEVNAM, mode ); }
   return ret;
}

static void __exit mopen_exit( void ) {
   LOG( "released device /dev/%s", DEVNAM );
   misc_deregister( &mopen_dev );
}

module_init( mopen_init );
module_exit( mopen_exit );
