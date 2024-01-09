CC        = gcc 
CFLAGS   += -O2 -Wall `pkg-config fuse --cflags`
#CFLAGS   += -w `pkg-config fuse --cflags`
LDFLAGS  += `pkg-config fuse --libs`
CPPFLAGS +=

%:    %.c 
	$(CC) $(CFLAGS)  $< $(LDFLAGS) -o $@

PROGLIST = fuse

all:  $(PROGLIST) clean

clean:
	@rm -f *.o

disclean: clean 
	@rm -f $(PROGLIST)
