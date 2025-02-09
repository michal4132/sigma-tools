CC = ./work/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
CFLAGS = -O2 -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables
LDFLAGS = -Wl,--gc-sections -Wl,--strip-all
GIT_VERSION := ${shell git describe --tags 2>/dev/null || echo "v0.0.1"}

-include work/device.mk

.DEFAULT_GOAL := user0.img
.PHONY: all clean upload debug web-deps web-build web-dev

BINARIES = alt_app/socketbridge alt_app/disable_led alt_app/httpd

all: user0.img

alt_app/socketbridge: socketbridge/main.c
	@mkdir -p alt_app
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -DVERSION=\"$(GIT_VERSION)\"

alt_app/httpd: httpd/main.c
	@mkdir -p alt_app
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -DVERSION=\"$(GIT_VERSION)\"

alt_app/disable_led: disable_led/main.c
	@mkdir -p alt_app
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -DVERSION=\"$(GIT_VERSION)\"

playground: playground.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -DVERSION=\"$(GIT_VERSION)\"

alt_app/.binaries_timestamp: $(BINARIES)
	@touch $@

user0.img: alt_app/.binaries_timestamp web-build
ifndef PARTITION_KEY
	$(error PARTITION_KEY not defined. Please create work/device.mk with PARTITION_KEY, IP and PASSWORD definitions)
endif
	./pack_payload.sh -i alt_app -o $@ -k $(PARTITION_KEY)

upload: user0.img
ifndef IP
	$(error IP not defined. Please create work/device.mk with PARTITION_KEY, IP and PASSWORD definitions)
endif
ifndef PASSWORD
	$(error PASSWORD not defined. Please create work/device.mk with PARTITION_KEY, IP and PASSWORD definitions)
endif
	cat $< | SSHPASS=$(PASSWORD) sshpass -e ssh root@$(IP) "cat >/tmp/user0.img"

debug: playground
ifndef IP
	$(error IP not defined. Please create work/device.mk with PARTITION_KEY, IP and PASSWORD definitions)
endif
ifndef PASSWORD
	$(error PASSWORD not defined. Please create work/device.mk with PARTITION_KEY, IP and PASSWORD definitions)
endif
	cat $< | SSHPASS=$(PASSWORD) sshpass -e ssh root@$(IP) "cat >/tmp/playground"

# Web application targets
web-deps:
	cd www && npm install

web-build: web-deps
	cd www && npm run build

web-dev: web-deps
	cd www && npm start

clean:
	rm -f $(BINARIES) user0.img playground alt_app/.binaries_timestamp
	rm -rf alt_app/www www/node_modules