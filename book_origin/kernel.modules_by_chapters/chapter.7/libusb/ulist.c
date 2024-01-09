#include <stdio.h>
#include <sys/types.h>
#include <libusb.h>
 
static void print_devs(libusb_device **devs) {
   libusb_device *dev;
   int i = 0;
   while ((dev = devs[i++]) != NULL) {
      struct libusb_device_descriptor desc;
      uint8_t path[8];
      int r = libusb_get_device_descriptor(dev, &desc);
      if (r < 0) {
         fprintf(stderr, "failed to get device descriptor");
         return;
      }
      printf("%04x:%04x (bus %d, device %d)",
             desc.idVendor, desc.idProduct,
             libusb_get_bus_number(dev), libusb_get_device_address(dev));
      r = libusb_get_port_numbers(dev, path, sizeof(path));
      if (r > 0) {
         int j;
         printf(" path: %d", path[0]);
         for (j = 1; j < r; j++)
            printf(".%d", path[j]);
      }
      printf("\n");
   }
}
 
int main(void) {
   libusb_device **devs;
   int r;
   ssize_t cnt;
 
   r = libusb_init(NULL);
   if (r < 0)
      return r;
   cnt = libusb_get_device_list(NULL, &devs);
   if (cnt < 0)
      return (int) cnt;
   print_devs(devs);
   libusb_free_device_list(devs, 1);
   libusb_exit(NULL);
   return 0;
}
