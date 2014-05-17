
ALL = \
      mbtcp \
      mbrtu \
      mbasc

all:
	for i in $(ALL); do \
		cd $$i; make ARCH=$(ARCH); cd ..; \
	done

clean:
	for i in $(ALL); do \
		cd $$i; make clean ARCH=$(ARCH); cd ..; \
	done
