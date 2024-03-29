#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "ioctl.h"

#define ERR(...) fprintf( stderr, __VA_ARGS__ ), exit( EXIT_FAILURE )

int main(int argc, char *argv[]) {
   int dfd;                  // дескриптор устройства 
   if((dfd = open(DEVPATH, O_RDWR)) < 0) ERR("Open device error: %m\n");
   RETURN_STRING buf;
   if(ioctl(dfd, IOCTL_GET_STRING, &buf)) ERR("IOCTL_GET_STRING error: %m\n");
   fprintf(stdout, "%s", (char*)&buf);
   close(dfd);
   return EXIT_SUCCESS;
}

