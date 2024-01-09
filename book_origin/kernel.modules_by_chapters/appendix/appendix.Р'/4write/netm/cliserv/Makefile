CC      = gcc
CFLAGS	= -O2 -Wall
OBJS	= writen.o readline.o strsock.o
HEAD    = common.h

%.o:    %.c $(HEAD)
	$(CC) $(CFLAGS) -c $< -o $@ 

all:	tcpserv tcpcli
	rm *.o

tcpserv: tcpserv.c $(HEAD) $(OBJS)
	$(CC) $(CFLAGS) $< -o $@ $(OBJS)

tcpcli: tcpcli.c $(HEAD) $(OBJS)
	$(CC) $(CFLAGS) $< -o $@ $(OBJS)

clean disclean:
	rm -f *.o core a.out temp*.* tcpserv tcpcli 
