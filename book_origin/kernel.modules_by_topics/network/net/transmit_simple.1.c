/*
 * (c) Copyright Jerry Cooperstein, 2009
 * (c) Copyright Oleg Tsiliuric, 2011-2022
 */

/*
 * Building a Transmitting Network Driver (simple solution)
 *
 * Extend your stub network device driver to include a transmission
 * function, which means supplying a method for
 * dev->hard_start_xmit().
 *
 * While you are at it, you may want to add other entry points to see
 * how you may exercise them.
 *
 * Once again, you should be able to exercise it with:
 *
 *   insmod transmit_simple.ko
 *   ifconfig mynet0 up 192.168.3.197
 *   ping -I mynet0 localhost
 *       or
 *   ping -bI mynet0 192.168.3
 *
 * Make sure your chosen address is not being used by anything else.
 */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/netdevice.h>

static int debug = 0;               // целочисленный парамер
module_param(debug, int, 0);
#define LOG(...) if(debug != 0) printk(KERN_INFO "! "__VA_ARGS__)

static struct net_device *dev;

static int my_hard_start_xmit(struct sk_buff *skb, struct net_device *dev) {
   struct netdev_queue *pnq;
//static inline struct netdev_queue *skb_get_tx_queue(const struct net_device *dev,
//                            const struct sk_buff *skb)
   pnq = skb_get_tx_queue(dev, skb);
   pnq->trans_start = jiffies;
   LOG("my_hard_start_xmit(%s) - sending packet :\n", dev->name);   
//   printk(KERN_INFO "! Sending packet :\n");
   if(debug > 1) {
//#define LINE 16
      char line[3 * 16 + 1], *plin = (char*)&line; 
//      printk(KERN_INFO "! my_hard_start_xmit(%s) - sending packet :\n", dev->name);
      /* print out 16 bytes per line */
      for(int i = 0; i < skb->len; ++i) {
         int pos = i % 16;
         if((i & 0xf) == 0)
//            LOG("%s\n", line);
            LOG("\n");
//         printk(KERN_INFO "%02x ", skb->data[i]);
         LOG("%02x ", skb->data[i]);
//         sprintf((char*)line + plin * 3, "%02x ", skb->data[i]);
      }
      LOG("\n");
   }
//   printk(KERN_INFO "\n");
   dev_kfree_skb(skb);
   return 0;
}

static int my_open(struct net_device *dev) {
   printk(KERN_INFO "! Hit: my_open(%s)\n", dev->name);
   /* start up the transmission queue */
   netif_start_queue(dev);
   return 0;
}

static int my_close(struct net_device *dev) {
   printk(KERN_INFO "! Hit: my_close(%s)\n", dev->name);
   /* shutdown the transmission queue */
   netif_stop_queue(dev);
   return 0;
}

static struct net_device_ops ndo = {
   .ndo_open = my_open,
   .ndo_stop = my_close,
   .ndo_start_xmit = my_hard_start_xmit,
};

static void my_setup(struct net_device *dev ) {
   /* Fill in the MAC address with a phoney */
   for(int j = 0; j < ETH_ALEN; ++j) 
      dev->dev_addr[j] = (char)j;
   ether_setup(dev);
   dev->netdev_ops = &ndo;
   dev->flags |= IFF_NOARP;
}

static int __init my_init(void) {
   printk(KERN_INFO "! Loading transmitting network module:....");
   printk(KERN_INFO "! debug level = %d", (int)debug);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0))
   dev = alloc_netdev(0, "mynet%d", my_setup);
#else
   dev = alloc_netdev(0, "mynet%d", NET_NAME_ENUM,  my_setup);
#endif
   if(register_netdev(dev)) {
      printk(KERN_INFO "! Failed to register\n");
      free_netdev(dev);
      return -1;
   }
   printk(KERN_INFO "! Succeeded in loading %s!\n", dev_name(&dev->dev));
   return 0;
}

static void __exit my_exit(void) {
   printk(KERN_INFO "! Unloading transmitting network module\n");
   unregister_netdev(dev);
   free_netdev(dev);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Bill Shubert");
MODULE_AUTHOR("Jerry Cooperstein");  // "LDD:1.0 s_26/lab1_transmit_simple.c"  
MODULE_AUTHOR("Oleg Tsiliuric");
MODULE_DESCRIPTION("");
MODULE_LICENSE("GPL v2");
