#include <stdio.h>
#include <libusb.h>

// usb_dev_handle* find_device ();

int main (int argc, char *argv[]) {
   const uint16_t vendor_id = 0x0624;
   const uint16_t product_id = 0x0249;
   struct libusb_device_handle *devh = NULL;
#define DATA_SIZE 8
   unsigned char buf[DATA_SIZE];
   int ret = libusb_init(NULL);
printf("init = %d\n", ret);

//   libusb_set_debug(NULL, USB_DEBUG_LEVEL);  // уровень вывода отладочных сообщений   
   libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
//int LIBUSB_CALL libusb_set_option(libusb_context *ctx, enum libusb_option option, ...);
//enum libusb_option {
//LIBUSB_OPTION_LOG_LEVEL,


   devh = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
   if (devh == NULL) { 
      printf("такое устройство не подключено\n");
      libusb_exit(NULL);
      return 1;
   }
#define DEV_INTF 1
   if (libusb_kernel_driver_active(devh, DEV_INTF)) 
      libusb_detach_kernel_driver(devh, DEV_INTF);
//   libusb_detach_kernel_driver(devh, 0);
   if (libusb_claim_interface(devh, DEV_INTF) < 0){ 
      printf("Ошибка интерфейса\n");
      return 2;
   }
//   libusb_claim_interface(devh, 0);
   int returned = libusb_interrupt_transfer(devh, 0x82, buf, DATA_SIZE, &ret, 5000);
printf("read = %d", ret); 
   if (returned >= 0) {
      printf("buf[0] = %d\n", (int)buf[0]);
      printf("buf[1] = %d\n", (int)buf[1]);
      printf("buf[2] = %d\n", (int)buf[2]);
      printf("buf[3] = %d\n", (int)buf[3]);      
   }
   libusb_release_interface(devh, DEV_INTF);
/*      cout << "buf[0] = " << (int)buf[0] << endl;
            cout << "buf[1] = " << (int)buf[1] << endl;
            cout << "buf[2] = " << (int)buf[2] << endl;
            cout << "buf[3] = " << (int)buf[3] << endl;
*/
/*
Всё это можно делать в бесконечном цикле. Для наглядности.
В первой строке мы читаем данные в буфер buf из устройства handle, 
через точку EP_IN (для мыши это 0x81).

Читаем DATA_SIZE байт. Причём int ret получит число реально считанных байт. 
А ждать данные мы будем не больше 1000мс.
Буфер я объявлял так:
*/

   libusb_attach_kernel_driver(devh, DEV_INTF);
   libusb_close(devh);
   libusb_exit(NULL);
   return 0;
}

/*
 const char * LIBUSB_CALL libusb_error_name(int errcode);
int LIBUSB_CALL libusb_setlocale(const char *locale);
const char * LIBUSB_CALL libusb_strerror(enum libusb_error errcode);

 */
