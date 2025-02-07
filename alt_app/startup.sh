#!/bin/sh

CURRENT_DIR=$PWD

NVRAM_PATH=/bin
GW_RUN_DIR=/tmp/tuya
GW_INIT_RUN_DIR=/tuya/app
GW_DATA_DIR=/tuya/data

APP_VERSION=$(cat $GW_INIT_RUN_DIR/version.txt)

[ ! -d /var/run ] && mkdir -p /var/run

killall udhcpc

echo root:dupa12 | chpasswd

# SSH
mkdir -p /etc/dropbear
dropbear -T 1 -p 22 -R

# mount -t sysfs sysfs /sys
eth_addr=$($NVRAM_PATH/nvram get master_mac)
if [ "$eth_addr" != "" ]; then
	ifconfig eth0 hw ether $eth_addr
else
    ifconfig eth0 hw ether 00e04c8196c5
fi

if [ -z "$APP_VERSION" ]; then
    echo "No APP Version"
    exit 1
fi

echo "Mod start"

# Disable LEDs
$GW_INIT_RUN_DIR/disable_led
echo 0 > /proc/tuya_netled_ctrl
echo 4 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio4/direction
echo 5 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio5/direction
echo 0 > /sys/class/gpio/gpio4/value
echo 0 > /sys/class/gpio/gpio5/value

rm -rf $GW_RUN_DIR
mkdir -p $GW_RUN_DIR

cp -r $GW_INIT_RUN_DIR/* $GW_RUN_DIR

echo "1" > /proc/sys/vm/overcommit_memory

# IP forwoard
echo "2" > /proc/sys/net/ipv6/conf/eth0/accept_ra
echo "64" > /proc/sys/net/ipv6/conf/eth0/accept_ra_rt_info_max_plen

echo "0" > /proc/sys/net/ipv6/conf/all/disable_ipv6
echo "2" > /proc/sys/net/ipv6/conf/all/accept_ra
echo "64" > /proc/sys/net/ipv6/conf/all/accept_ra_rt_info_max_plen
echo "1" > /proc/sys/net/ipv6/conf/all/forwarding
echo "1" > /proc/sys/net/ipv4/ip_forward

# GPIO for Zigbee/Thread module
echo 47 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio47/direction
echo 48 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio48/direction
echo 1 > /sys/class/gpio/gpio48/value

# Start apps
/tmp/tuya/dhcpc_srv start
/tmp/tuya/syslogd_srv start
/tmp/tuya/mdnsd_srv start
/tmp/tuya/socketbridge_srv start
/tmp/tuya/cpcd_srv start
/tmp/tuya/zigbeed_srv start

echo "End"
exit 0
