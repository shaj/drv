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
      for(int j = 0; j < ETH_ALEN; j++)
      sprintf(smac + j * 3, "%02x%c",
              dev->dev_addr[j], ETH_ALEN - 1 != j ? ':' : '\0' );
      printk(KERN_INFO "name=%6s irq=%3d MAC=%s\n",
                       dev->name, dev->irq, smac);
      {
         const struct net_device_ops *ndops = dev->netdev_ops;
	 struct rtnl_link_stats64 stat = {};
         printk(KERN_INFO "&net_device_ops=%px &ndo_get_stats=%px\n", 
                          ndops, ndops->ndo_get_stats64);  
         if (ndops->ndo_get_stats64 != NULL) {

            // ndops->ndo_get_stats64;
            ndops->ndo_get_stats64(dev, &stat);
         }
	 printk("rx_bytes=%8llu tx_bytes=%8llu\n",
                stat.rx_bytes, stat.tx_bytes);  
      }


//    __u64       rx_bytes;               /* total bytes received         */
//    __u64       tx_bytes;               /* total bytes transmitted      */


      dev = next_net_device(dev);
    }
    return -1;
}

//        printk(KERN_INFO "&net_device_ops=%px &net_device_stats=%px\n",
//               ndops, pstat);  

//        printk(KERN_INFO "MAC=%pM %d\n", dev->perm_addr, ETH_ALEN);
//        printk(KERN_INFO "MAC=% ");
//        printk(KERN_INFO "%ETH_ALENs", dev->dev_addr);
//        char smac[ETH_ALEN*3];
//        for(j = 0; j < ETH_ALEN; ++j)
//         smac[j * 3 - 1] = '\0';
//         printk(KERN_INFO "%s\n", smac);

//      printk(KERN_INFO "name = %6s irq=%4d\n", dev->name, dev->irq);

// printk(KERN_INFO "rx_bytes = %lu tx_bytes = %lu\n",
//       pstat->rx_bytes, pstat->tx_bytes);
//      const struct net_device_ops *netdev_ops;
//      struct net_device_stats *nds = ndops->ndo_get_stats(dev);
//              "name = %6s irq=%4d trans_start=%12lu last_rx=%12lu\n",
//             "name = %6s irq=%4d tx_bytes=%12lu rx_bytes=%12lu\n",
//              dev->name, dev->irq, dev->trans_start, dev->last_rx);
//             dev->name, dev->irq, nds->tx_bytes, nds->rx_bytes);
//             dev->name, dev->irq, dev->tx_dropped, dev->rx_dropped);


module_init(my_init);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LDD:1.0 s_25/lab1_devices.c");
MODULE_LICENSE("GPL v2");
