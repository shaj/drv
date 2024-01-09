/*
 * The code herein is: Copyright Jerry Cooperstein, 2009
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>

static int __init my_init(void) {
   struct net_device *dev;
   printk(KERN_INFO "Hello: module loaded at 0x%px\n", my_init);
   dev = first_net_device(&init_net);
   printk(KERN_INFO "Hello: dev_base address=0x%px\n", dev);
   while (dev) { 
      char smac[ETH_ALEN*3];
      struct rtnl_link_stats64 stat = {};
      for(int j = 0; j < ETH_ALEN; j++)
      sprintf(smac + j * 3, "%02x%c",
              dev->dev_addr[j], ETH_ALEN - 1 != j ? ':' : '\0' );
//      printk(KERN_INFO "name=%6s irq=%3d MAC=%s\n",
//                       dev->name, dev->irq, smac);
      {
         const struct net_device_ops *ndops = dev->netdev_ops;
//         printk(KERN_INFO "&net_device_ops=%px &ndo_get_stats=%px\n", 
//                          ndops, ndops->ndo_get_stats64);  
         if (ndops->ndo_get_stats64 != NULL) {

            // ndops->ndo_get_stats64;
            ndops->ndo_get_stats64(dev, &stat);
         }
//	 printk("rx_bytes=%8llu tx_bytes=%8llu\n",
//                stat.rx_bytes, stat.tx_bytes);  
      }
      printk(KERN_INFO "name=%-6s irq=%-3d MAC=%s "
                       "rx_bytes=%-8llu tx_bytes=%-8llu \n",
                       dev->name, dev->irq, smac,
		       stat.rx_bytes, stat.tx_bytes);

//printk("rx_bytes=%8llu tx_bytes=%8llu\n",
//              stat.rx_bytes, stat.tx_bytes);  

      dev = next_net_device(dev);
    }
    return -1;
}



module_init(my_init);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LDD:1.0 s_25/lab1_devices.c");
MODULE_LICENSE("GPL v2");
