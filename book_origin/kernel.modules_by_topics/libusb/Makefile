CC        = gcc
CFLAGS   += -O2 -Wall `pkg-config libusb-1.0 --cflags`
LDFLAGS  += `pkg-config libusb-1.0 --libs`

COPT = `pkg-config fuse --cflags --libs`

CPPFLAGS +=

%:    %.c 
	$(CC) $(CFLAGS)  $< $(LDFLAGS) -o $@

PROGLIST = utest ulist

all:  $(PROGLIST) clean

clean:
	@rm -f *.o

disclean: clean 
	@rm -f $(PROGLIST)
