SUBDIRS = $(shell find . -maxdepth 1 -mindepth 1 -type d -printf "%f\n")

all install uninstall clean disclean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	   echo "=============== making $@ in $$subdir ================="; \
	   (cd $$subdir && make $@) \
	done

arch: disclean
	rm -f *.tgz
	tar -czvf ../disfw.tgz * > /dev/null
