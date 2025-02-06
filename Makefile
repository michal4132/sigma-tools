CC=./work/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
CFLAGS=-O2 -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables
LDFLAGS=-Wl,--gc-sections -Wl,--strip-all
GIT_VERSION:=${shell git describe --tags 2>/dev/null || echo "v0.0.1"}

-include work/key.mk

.PHONY: pack build clean upload

pack: build
ifdef PARTITION_KEY
	./pack_payload.sh -i alt_app -o user0.img -k $(PARTITION_KEY)
else
	@echo "Error: PARTITION_KEY not defined. Please create work/key.mk with PARTITION_KEY definition"
	@exit 1
endif

build: alt_app/socketbridge

alt_app/socketbridge: socketbridge/main.c
	$(CC) $(CFLAGS) \
		socketbridge/main.c \
		-o alt_app/socketbridge \
		$(LDFLAGS) \
		-DVERSION=\"$(GIT_VERSION)\"

user0.img: alt_app/socketbridge
	$(MAKE) pack

upload: user0.img
	cat user0.img | ssh root@192.168.0.112 "cat >/tmp/user0.img"

clean:
	rm -f alt_app/socketbridge user0.img
