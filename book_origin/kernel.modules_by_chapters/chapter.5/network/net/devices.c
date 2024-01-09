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
      if (dev->netdev_ops->ndo_get_stats64 != NULL)
         dev->netdev_ops->ndo_get_stats64(dev, &stat);
      printk(KERN_INFO "name=%-6s irq=%-3d MAC=%s "
                       "rx_bytes=%-8llu tx_bytes=%-8llu \n",
                       dev->name, dev->irq, smac,
                       stat.rx_bytes, stat.tx_bytes);
      dev = next_net_device(dev);
    }
    return -1;
}

module_init(my_init);

MODULE_AUTHOR("Oleg Tsiliuric");
MODULE_LICENSE("GPL v2");
