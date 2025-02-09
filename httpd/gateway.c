#include "handlers.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>

// Get system uptime
static char* get_uptime_string(void) {
    struct sysinfo si;
    if (sysinfo(&si) != 0) {
        return strdup("unknown");
    }

    long days = si.uptime / (24 * 3600);
    long hours = (si.uptime % (24 * 3600)) / 3600;
    long mins = (si.uptime % 3600) / 60;

    char* result = malloc(32);
    if (days > 0) {
        snprintf(result, 32, "%ldd %ldh %ldm", days, hours, mins);
    } else if (hours > 0) {
        snprintf(result, 32, "%ldh %ldm", hours, mins);
    } else {
        snprintf(result, 32, "%ldm", mins);
    }
    return result;
}

// Get IP address for interface
static char* get_ip_address(const char* interface) {
    int fd;
    struct ifreq ifr;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        return strdup("unknown");
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
    
    if (ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
        close(fd);
        return strdup("unknown");
    }
    
    close(fd);
    return strdup(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

enum MHD_Result handle_gateway_status(struct MHD_Connection *connection) {
    struct json_object *root = json_object_new_object();
    if (!root) {
        return send_error_response(connection, "GET", "/api/gateway/status", 
            "Failed to create JSON response", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }

    // Create gateway object
    struct json_object *gateway = json_object_new_object();
    if (!gateway) {
        json_object_put(root);
        return send_error_response(connection, "GET", "/api/gateway/status", 
            "Failed to create gateway object", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }
    json_object_object_add(gateway, "status", json_object_new_string("online"));
    
    // Get IP address
    char* ip = get_ip_address("eth0");
    json_object_object_add(gateway, "ip", json_object_new_string(ip));
    free(ip);

    // Add uptime
    char* uptime = get_uptime_string();
    json_object_object_add(gateway, "uptime", json_object_new_string(uptime));
    free(uptime);

    // Add version from uname
    struct utsname sys_info;
    if (uname(&sys_info) == 0) {
        json_object_object_add(gateway, "version", json_object_new_string(sys_info.release));
    } else {
        json_object_object_add(gateway, "version", json_object_new_string("unknown"));
    }

    json_object_object_add(root, "gateway", gateway);

    // Create network object
    struct json_object *network = json_object_new_object();
    if (!network) {
        json_object_put(root);
        return send_error_response(connection, "GET", "/api/gateway/status", 
            "Failed to create network object", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }

    // Zigbee network status
    struct json_object *zigbee = json_object_new_object();
    if (!zigbee) {
        json_object_put(root);
        return send_error_response(connection, "GET", "/api/gateway/status", 
            "Failed to create Zigbee status object", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }
    json_object_object_add(zigbee, "status", json_object_new_string("active"));
    json_object_object_add(zigbee, "signal_strength", json_object_new_int(85));
    json_object_object_add(zigbee, "channel", json_object_new_int(15));
    json_object_object_add(zigbee, "pan_id", json_object_new_string("0x1A2B"));
    json_object_object_add(network, "zigbee", zigbee);

    // Matter network status
    struct json_object *matter = json_object_new_object();
    if (!matter) {
        json_object_put(root);
        return send_error_response(connection, "GET", "/api/gateway/status", 
            "Failed to create Matter status object", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }
    json_object_object_add(matter, "status", json_object_new_string("active"));
    json_object_object_add(matter, "fabric_id", json_object_new_string("0xABCD1234"));
    json_object_object_add(network, "matter", matter);

    json_object_object_add(root, "network", network);

    // Devices array (mock data for now)
    struct json_object *devices = json_object_new_array();
    if (!devices) {
        json_object_put(root);
        return send_error_response(connection, "GET", "/api/gateway/status", 
            "Failed to create devices array", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }

    // Example devices (matching mock data)
    struct json_object *device1 = json_object_new_object();
    if (device1) {
        json_object_object_add(device1, "id", json_object_new_string("1"));
        json_object_object_add(device1, "name", json_object_new_string("Living Room Light"));
        json_object_object_add(device1, "type", json_object_new_string("Light"));
        json_object_object_add(device1, "protocol", json_object_new_string("zigbee"));
        json_object_object_add(device1, "status", json_object_new_string("online"));
        json_object_object_add(device1, "last_seen", json_object_new_string("2 min ago"));
        json_object_array_add(devices, device1);
    }

    struct json_object *device2 = json_object_new_object();
    if (device2) {
        json_object_object_add(device2, "id", json_object_new_string("2"));
        json_object_object_add(device2, "name", json_object_new_string("Kitchen Sensor"));
        json_object_object_add(device2, "type", json_object_new_string("Motion Sensor"));
        json_object_object_add(device2, "protocol", json_object_new_string("zigbee"));
        json_object_object_add(device2, "status", json_object_new_string("online"));
        json_object_object_add(device2, "battery", json_object_new_int(85));
        json_object_object_add(device2, "last_seen", json_object_new_string("5 min ago"));
        json_object_array_add(devices, device2);
    }

    struct json_object *device3 = json_object_new_object();
    if (device3) {
        json_object_object_add(device3, "id", json_object_new_string("3"));
        json_object_object_add(device3, "name", json_object_new_string("Door Lock"));
        json_object_object_add(device3, "type", json_object_new_string("Lock"));
        json_object_object_add(device3, "protocol", json_object_new_string("matter"));
        json_object_object_add(device3, "status", json_object_new_string("online"));
        json_object_object_add(device3, "battery", json_object_new_int(90));
        json_object_object_add(device3, "last_seen", json_object_new_string("1 min ago"));
        json_object_array_add(devices, device3);
    }

    json_object_object_add(root, "devices", devices);

    const char *json_str = json_object_to_json_string(root);
    if (!json_str) {
        json_object_put(root);
        return send_error_response(connection, "GET", "/api/gateway/status", 
            "Failed to serialize JSON response", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }

    enum MHD_Result ret = send_json_response(connection, "GET", "/api/gateway/status", json_str, MHD_HTTP_OK);
    json_object_put(root);
    return ret;
}
