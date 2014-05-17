
ALL = src samples

all:
	for i in $(ALL); do \
		cd $$i; make ARCH=$(ARCH); cd ..; \
	done

clean:
	for i in $(ALL); do \
		cd $$i; make clean ARCH=$(ARCH); cd ..; \
	done

dist:
	tar cvzf /tmp/moxalib.tar.gz src samples inc lib rules.make makefile
