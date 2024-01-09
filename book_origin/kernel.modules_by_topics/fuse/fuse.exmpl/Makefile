CC = gcc -Wall 
COPT = `pkg-config fuse --cflags --libs` 
PROGS = hello cusexmp fsel fselclient xmp fusexmp fusexmp_fh

%: %.c
	$(CC) $< $(COPT) -o $@

all:    $(PROGS)

fusexmp_fh: fusexmp_fh.c
	$(CC) $< $(COPT) -lulockmgr -o $@

clean disclean:
	rm -f $(PROGS) *.o
