
MXLIB = mxphio

all:
	for i in $(MXLIB); do \
		cd $$i; make ARCH=$(ARCH); cd ..; \
	done

clean:
	for i in $(MXLIB); do \
		cd $$i; make clean ARCH=$(ARCH); cd ..; \
	done
