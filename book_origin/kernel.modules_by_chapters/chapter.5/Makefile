#SUBDIRS = $(shell ls -l | awk '/^d/ { print $$9 }')
SUBDIRS=$(shell find . -maxdepth 1 -mindepth 1 -type d -printf "%f\n")
KARCH=kernel.modules.tgz

all clean disclean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	   echo "=============== making $@ in $$subdir ================="; \
	   (cd $$subdir && make $@) \
	done

tree:
	tree > tree.txt

arch:	disclean tree
	@rm -f *.tgz
	echo ================ $(KARCH) =========================
	tar -czf $(KARCH) *
	mv $(KARCH) ..
