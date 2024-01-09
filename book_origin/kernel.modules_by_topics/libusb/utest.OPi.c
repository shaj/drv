#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <libusb.h>

int main (int argc, char *argv[]) {
   const uint16_t vendor_id = 0x0557;
   const uint16_t product_id = 0x0204;
   struct libusb_device_handle *devh = NULL;
#define SIZE 8
   unsigned char buf[SIZE];
   int ret, debug = 0, rep = 5;
   while ((ret = getopt(argc, argv, "v")) != -1) {
      switch (ret) {
         case 'v':
            debug++;
      }
   }
   if (argc > optind && atoi(argv[optind]) > 0)
      rep = atoi(argv[optind]);
   if ((ret = libusb_init(NULL)) != 0) {
      fprintf(stderr, "ошибка инициализации libusb: %d\n", ret);
      return 2;
   }
   if (debug > 0)  // уровень вывода отладочных сообщений   
      libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
   devh = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
   if (devh == NULL) { 
      fprintf(stderr, "такое устройство не подключено: %X:%X\n", 
              vendor_id, product_id);
      libusb_exit(NULL);
      return 3;
   }
#define DEV_INTF 1 // bInterfaceNumber
#define EP 0x82    // bEndpointAddress
   if (libusb_kernel_driver_active(devh, DEV_INTF)) 
      libusb_detach_kernel_driver(devh, DEV_INTF);
   if (libusb_claim_interface(devh, DEV_INTF) < 0){ 
      fprintf(stderr, "ошибка интерфейса\n");
      return 4;
   }
   while (rep-- > 0) {
      int i, 
         returned = libusb_interrupt_transfer(devh, EP, buf, SIZE, &ret, 5000);
      if (returned >= 0) {
         printf("read %d байт: ", ret);
         for (i = 0; i < ret; i++)
            printf("%d ", (int)buf[i]);
         printf("\n");
      }
      else {
		 printf("непонятно ... тайм-аут?\n");
	  }
   }
   libusb_release_interface(devh, DEV_INTF);
   libusb_attach_kernel_driver(devh, DEV_INTF);
   libusb_close(devh);
   libusb_exit(NULL);
   return 0;
}
