#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/init.h>

#define LEN_MSG 160
static char buf_msg[ LEN_MSG + 1 ] = "Hello from module!\n";

/* <linux/device.h>
LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32)
struct class_attribute {
   struct attribute attr;
   ssize_t (*show)(struct class *class, struct class_attribute *attr, char *buf);
   ssize_t (*store)(struct class *class, struct class_attribute *attr,
                    const char *buf, size_t count);
};
LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32)
struct class_attribute {
   struct attribute attr;
   ssize_t (*show)(struct class *class, char *buf);
   ssize_t (*store)(struct class *class, const char *buf, size_t count);
};
*/

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
/*
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32)
static ssize_t x_show(struct class *class, struct class_attribute *attr, char *buf) {
#else
static ssize_t x_show(struct class *class, char *buf) {
#endif
*/
static ssize_t x_show(struct device *device, struct device_attribute *attr, char *buf) {
   strcpy(buf, buf_msg);
   printk("read %ld\n", (long)strlen(buf));
   return strlen(buf);
}
// ssize_t (*)(struct device *, struct device_attribute *, char *)
// {aka ‘long int (*)(struct device *, struct device_attribute *, char *)’} 
// from incompatible pointer type ‘ssize_t (*)(struct class *, struct class_attribute *, char *)’ 
// {aka ‘long int (*)(struct class *, struct class_attribute *, char *)’


/* sysfs store() method. Calls the store() method corresponding to the individual sysfs file */
/*
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32)
static ssize_t x_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) {
#else
static ssize_t x_store(struct class *class, const char *buf, size_t count) {
#endif
*/
ssize_t x_store(struct device *device, struct device_attribute *attr, const char *buf, size_t count) {
   printk("write %ld\n", (long)count);
   strncpy(buf_msg, buf, count);
   buf_msg[ count ] = '\0';
   return count;
}
//‘ssize_t (*)(struct device *, struct device_attribute *, const char *, size_t)
//’ {aka ‘long int (*)(struct device *, struct device_attribute *, const char *, long unsigned int)’}



/* <linux/device.h>
#define CLASS_ATTR(_name, _mode, _show, _store) \
struct class_attribute class_attr_##_name = __ATTR(_name, _mode, _show, _store) */
//CLASS_ATTR(xxx, (S_IWUSR | S_IRUGO), &x_show, &x_store);
DEVICE_ATTR(xxx, (S_IWUSR | S_IRUGO), &x_show, &x_store);

static struct class *x_class;

int __init x_init(void) {
   int res;
   x_class = class_create(THIS_MODULE, "x-class");
   if(IS_ERR(x_class)) printk("bad class create\n");
//   res = class_create_file(x_class, &class_attr_xxx);
   res = class_create_file(x_class, &dev_attr_xxx);
/* <linux/device.h>
extern int __must_check class_create_file(struct class *class, const struct class_attribute *attr); */
   printk("'xxx' module initialized\n");
   return res;
}

void x_cleanup(void) {
/* <linux/device.h>
extern void class_remove_file(struct class *class, const struct class_attribute *attr); */
//   class_remove_file(x_class, &class_attr_xxx);
   class_remove_file(x_class, &dev_attr_xxx);
   class_destroy(x_class);
   return;
}

module_init(x_init);
module_exit(x_cleanup);
MODULE_LICENSE("GPL");
