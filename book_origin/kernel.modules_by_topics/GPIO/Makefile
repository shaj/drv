CC        = gcc -Wall
CFLAGS   += -O2 -Wall
LDFLAGS  +=
CPPFLAGS +=

%:    %.c 
	$(CC) $(CFLAGS) $< -o $@

PROGLIST = info allops

all:  $(PROGLIST) clean

clean:
	@rm -f *.o

disclean: clean 
	@rm -f $(PROGLIST)
