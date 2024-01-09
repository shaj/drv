#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/gpio.h>

#define DEV_NAME "/dev/gpiochip0"

int main(int argc, char* argv[]) {
   int fd, ret, debug = 0;
   struct gpiochip_info info;
   struct gpioline_info line_info;
   char dev_name[80];
   while ((ret = getopt(argc, argv, "v")) != -1) {
      switch (ret) {
         case 'v':
            debug++;
            break;
         default:
            fprintf(stderr,
                    "Usage: %s options dev_name.\n"
                    "Options:\n"
                    "\t -v increase output level\n",
                    argv[0]);
            return -1;
      }
   }
   strcat(dev_name, 1 == argc ? DEV_NAME : argv[optind]);
   fd = open(dev_name, O_RDONLY);
   if (fd < 0) {
      printf("Unabled to open %s: %s\n", dev_name, strerror(errno));
      return 1;
   }
   // Query GPIO chip information
   ret = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &info);
   if (ret == -1) {
      printf("Unable to get chip info from ioctl: %s", strerror(errno));
      close(fd);
      return 2;
   }
   printf("Chip name: %s\n", info.name);
   printf("Chip label: %s\n", info.label);
   printf("Number of lines: %d\n", info.lines);
   if (0 == debug)
      return 0;
   for (int i = 0; i < info.lines; i++) {
      line_info.line_offset = i;
      ret = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &line_info);
      if (ret == -1) {
         printf("Unable to get line info from offset %d: %s", i, strerror(errno));
      }
      else {
         printf("offset: %d, name: %s, consumer: %s. Flags:\t[%s]\t[%s]\t[%s]\t[%s]\t[%s]\n",
                i,
                line_info.name,
                line_info.consumer,
                (line_info.flags & GPIOLINE_FLAG_IS_OUT) ? "OUTPUT" : "INPUT",
                (line_info.flags & GPIOLINE_FLAG_ACTIVE_LOW) ? "ACTIVE_LOW" : "ACTIVE_HIGHT",
                (line_info.flags & GPIOLINE_FLAG_OPEN_DRAIN) ? "OPEN_DRAIN" : "...",
                (line_info.flags & GPIOLINE_FLAG_OPEN_SOURCE) ? "OPENSOURCE" : "...",
                (line_info.flags & GPIOLINE_FLAG_KERNEL) ? "KERNEL" : "");
      }
   }
   (void)close(fd);
   return 0;
}
