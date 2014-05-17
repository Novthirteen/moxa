
ALL = \
      connection_serial \
      connection_tcp \
      connection_udp \
      connection_udp_broadcast

all:
	for i in $(ALL); do \
		cd $$i; make ARCH=$(ARCH); cd ..; \
	done

clean:
	for i in $(ALL); do \
		cd $$i; make clean ARCH=$(ARCH); cd ..; \
	done
