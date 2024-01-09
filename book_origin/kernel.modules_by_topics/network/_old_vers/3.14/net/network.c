/*
 * The code herein is: Copyright Jerry Cooperstein, 2009
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 */
#include <linux/module.h>
#include <linux/netdevice.h>

static struct net_device *dev;

static int my_open( struct net_device *dev ) {
   printk( KERN_INFO "Hit: my_open(%s)\n", dev->name );
   /* start up the transmission queue */
   netif_start_queue( dev );
   return 0;
}

static int my_close( struct net_device *dev ) {
   printk( KERN_INFO "Hit: my_close(%s)\n", dev->name );
   /* shutdown the transmission queue */
   netif_stop_queue( dev );
   return 0;
}

/* Note this method is only needed on some; without it
   module will fail upon removal or use. At any rate there is a memory
   leak whenever you try to send a packet through in any case*/
static int stub_start_xmit( struct sk_buff *skb, struct net_device *dev ) {
   dev_kfree_skb( skb );
   return 0;
}

static struct net_device_ops ndo = {
   .ndo_open = my_open,
   .ndo_stop = my_close,
   .ndo_start_xmit = stub_start_xmit,
};

static void my_setup( struct net_device *dev ) {
   int j;
   printk( KERN_INFO "my_setup(%s)\n", dev->name );
   /* Fill in the MAC address with a phoney */
   for( j = 0; j < ETH_ALEN; ++j ) 
      dev->dev_addr[ j ] = (char)j;
   ether_setup( dev );
   dev->netdev_ops = &ndo;
}

static int __init my_init( void ) {
   printk( KERN_INFO "Loading stub network module:...." );
   dev = alloc_netdev( 0, "fict%d", my_setup );
   if( register_netdev( dev ) ) {
      printk( KERN_INFO " Failed to register\n" );
      free_netdev( dev );
      return -1;
   }
   printk( KERN_INFO "Succeeded in loading %s!\n", dev_name( &dev->dev ) );
   return 0;
}

static void __exit my_exit( void ) {
   printk( KERN_INFO "Unloading stub network module\n" );
   unregister_netdev( dev );
   free_netdev( dev );
}

module_init( my_init );
module_exit( my_exit );

MODULE_AUTHOR( "Bill Shubert" );
MODULE_AUTHOR( "Jerry Cooperstein" );
MODULE_AUTHOR( "Tatsuo Kawasaki" );
MODULE_DESCRIPTION( "LDD:1.0 s_24/lab1_network.c" );
MODULE_LICENSE( "GPL v2" );
