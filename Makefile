ROOT_DIR := $(shell pwd)
TOOLCHAIN_DIR ?= $(ROOT_DIR)/work/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin
TOOLCHAIN_PREFIX ?= arm-linux-gnueabihf-
PREFIX ?= $(ROOT_DIR)/work/rootfs

CC = $(TOOLCHAIN_DIR)/$(TOOLCHAIN_PREFIX)gcc
CXX = $(TOOLCHAIN_DIR)/$(TOOLCHAIN_PREFIX)g++
LD = $(TOOLCHAIN_DIR)/$(TOOLCHAIN_PREFIX)ld
CFLAGS = -O2 -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables
LDFLAGS = -Wl,--gc-sections -Wl,--strip-all
GIT_VERSION := ${shell git describe --tags 2>/dev/null || echo "v0.0.1"}

-include work/device.mk

.DEFAULT_GOAL := user0.img
.PHONY: all clean upload debug setup web-deps web-build web-dev web-upload debug-upload check-env

BINARIES = alt_app/socketbridge alt_app/disable_led alt_app/httpd

all: user0.img

alt_app/socketbridge: socketbridge/main.c
	@mkdir -p alt_app
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -DVERSION=\"$(GIT_VERSION)\"

alt_app/disable_led: disable_led/main.c
	@mkdir -p alt_app
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -DVERSION=\"$(GIT_VERSION)\"

alt_app/httpd: httpd/main.c httpd/gateway.c httpd/settings.c httpd/logs.c httpd/utils.c httpd/handlers.h
	@mkdir -p alt_app
	$(CC) $(CFLAGS) httpd/main.c httpd/gateway.c httpd/settings.c httpd/logs.c httpd/utils.c -o $@ $(LDFLAGS) -DVERSION=\"$(GIT_VERSION)\" \
		-I$(PREFIX)/include -L$(PREFIX)/lib -Wl,-Bstatic -lmicrohttpd -ljson-c -Wl,-Bdynamic -lpthread

playground: playground.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -DVERSION=\"$(GIT_VERSION)\" \
		-I$(PREFIX)/include -L$(PREFIX)/lib -Wl,-Bstatic -lmicrohttpd -ljson-c -Wl,-Bdynamic -lpthread

alt_app/.binaries_timestamp: $(BINARIES)
	@touch $@

user0.img: alt_app/.binaries_timestamp web-build
ifndef PARTITION_KEY
	$(error PARTITION_KEY not defined. Please create work/device.mk with PARTITION_KEY, IP and PASSWORD definitions)
endif
	./pack_payload.sh -i alt_app -o $@ -k $(PARTITION_KEY)

check-env:
ifndef IP
	$(error IP not defined. Please create work/device.mk with PARTITION_KEY, IP and PASSWORD definitions)
endif
ifndef PASSWORD
	$(error PASSWORD not defined. Please create work/device.mk with PARTITION_KEY, IP and PASSWORD definitions)
endif

upload: user0.img check-env
	cat $< | SSHPASS=$(PASSWORD) sshpass -e ssh root@$(IP) "cat >/tmp/user0.img"

debug: playground check-env
	cat $< | SSHPASS=$(PASSWORD) sshpass -e ssh root@$(IP) "cat >/tmp/playground"

setup:
	wget https://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.77.tar.gz
	tar xvf libmicrohttpd-0.9.77.tar.gz
	cd libmicrohttpd-0.9.77 && \
	./configure --host=arm-linux-gnueabihf \
		CC=$(CC) CXX=$(CXX) LD=$(LD) --prefix=$(PREFIX) \
		--enable-static --disable-shared && \
	make -j4 && \
	make install
	rm -rf libmicrohttpd-0.9.77 libmicrohttpd-0.9.77.tar.gz

	# Install json-c
	wget https://s3.amazonaws.com/json-c_releases/releases/json-c-0.16.tar.gz
	tar xvf json-c-0.16.tar.gz
	cd json-c-0.16 && \
	mkdir -p build && cd build && \
	cmake -DCMAKE_C_COMPILER=$(CC) \
		-DCMAKE_INSTALL_PREFIX=$(PREFIX) \
		-DBUILD_SHARED_LIBS=ON \
		-DCMAKE_SYSTEM_NAME=Linux \
		-DCMAKE_SYSTEM_PROCESSOR=arm \
		.. && \
	make -j4 && \
	make install
	rm -rf json-c-0.16 json-c-0.16.tar.gz

	cd www && npm install

web-build: web-deps
	cd www && npm run build

web-debug: web-build check-env
	cd alt_app/www && tar czf - . | SSHPASS=$(PASSWORD) sshpass -e ssh root@$(IP) "cd /tmp/tuya/www && tar xzf -"

web-dev: web-deps
	cd www && npm start

httpd-debug: alt_app/httpd check-env
	cat alt_app/httpd | SSHPASS=$(PASSWORD) sshpass -e ssh root@$(IP) "cat >/tmp/httpd"

clean:
	rm -f $(BINARIES) user0.img playground alt_app/.binaries_timestamp
	rm -rf alt_app/www www/node_modules
	rm -rf libmicrohttpd-0.9.77 libmicrohttpd-0.9.77.tar.gz
	rm -rf json-c-0.16 json-c-0.16.tar.gz
