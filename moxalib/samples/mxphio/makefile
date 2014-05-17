
ALL = \
      buzzer \
      digit_input_change \
      keypad_callback \
      keypad_lcm \
      lcm \
      net_iface \
      rtc \
      serial \
      socket_tcp \
      socket_udp \
      watch_dog

all:
	for i in $(ALL); do \
		cd $$i; make ARCH=$(ARCH); cd ..; \
	done

clean:
	for i in $(ALL); do \
		cd $$i; make clean ARCH=$(ARCH); cd ..; \
	done
