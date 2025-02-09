#include "handlers.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <stdbool.h>

// Forward declarations of static functions
static char* read_post_data(struct MHD_Connection *connection, 
                          const char *upload_data,
                          size_t *upload_data_size, 
                          void **con_cls);

enum MHD_Result handle_settings_get(struct MHD_Connection *connection) {
    struct json_object *root = json_object_new_object();
    
    // Network settings
    struct json_object *network = json_object_new_object();
    json_object_object_add(network, "hostname", json_object_new_string("gateway-01"));
    json_object_object_add(network, "dhcp", json_object_new_boolean(true));
    json_object_object_add(network, "ip", json_object_new_string("192.168.1.100"));
    json_object_object_add(network, "netmask", json_object_new_string("255.255.255.0"));
    json_object_object_add(network, "gateway", json_object_new_string("192.168.1.1"));
    json_object_object_add(network, "dns_primary", json_object_new_string("8.8.8.8"));
    json_object_object_add(network, "dns_secondary", json_object_new_string("8.8.4.4"));
    
    // Zigbee settings
    struct json_object *zigbee = json_object_new_object();
    json_object_object_add(zigbee, "enabled", json_object_new_boolean(true));
    json_object_object_add(zigbee, "channel", json_object_new_int(11));
    json_object_object_add(zigbee, "pan_id", json_object_new_string("0x1A2B"));
    json_object_object_add(zigbee, "permit_join", json_object_new_boolean(false));
    
    // Matter settings
    struct json_object *matter = json_object_new_object();
    json_object_object_add(matter, "enabled", json_object_new_boolean(true));
    json_object_object_add(matter, "fabric_id", json_object_new_string("12345"));
    json_object_object_add(matter, "commission_mode", json_object_new_boolean(false));
    
    // Hardware settings
    struct json_object *hardware = json_object_new_object();
    json_object_object_add(hardware, "status_led", json_object_new_boolean(true));
    json_object_object_add(hardware, "network_led", json_object_new_boolean(true));
    
    // System settings
    struct json_object *system = json_object_new_object();
    json_object_object_add(system, "name", json_object_new_string("Smart Home Gateway"));
    json_object_object_add(system, "timezone", json_object_new_string("Europe/London"));
    json_object_object_add(system, "log_level", json_object_new_string("info"));
    json_object_object_add(system, "ssh_enabled", json_object_new_boolean(true));
    json_object_object_add(system, "ssh_port", json_object_new_int(22));
    
    json_object_object_add(root, "network", network);
    json_object_object_add(root, "zigbee", zigbee);
    json_object_object_add(root, "matter", matter);
    json_object_object_add(root, "hardware", hardware);
    json_object_object_add(root, "system", system);
    
    const char *json_str = json_object_to_json_string(root);
    enum MHD_Result ret = send_json_response(connection, "GET", "/api/settings", json_str, MHD_HTTP_OK);
    
    json_object_put(root);
    return ret;
}

enum MHD_Result handle_settings_patch(struct MHD_Connection *connection,
                                    const char *upload_data,
                                    size_t *upload_data_size,
                                    void **con_cls) {
    char *post_data = read_post_data(connection, upload_data, upload_data_size, con_cls);
    if (post_data == NULL) {
        return MHD_YES;  // Not ready to process yet
    }

    // Parse JSON data
    struct json_object *root = json_tokener_parse(post_data);
    free(post_data);

    if (root == NULL) {
        const char *error_msg = "Invalid JSON data";
        return send_json_response(connection, "PATCH", "/api/settings", error_msg, MHD_HTTP_BAD_REQUEST);
    }

    // TODO: Apply settings changes here
    // For now, just acknowledge receipt
    json_object_put(root);
    return send_json_response(connection, "PATCH", "/api/settings", "{\"status\":\"ok\"}", MHD_HTTP_OK);
}

static char* read_post_data(struct MHD_Connection *connection, 
                          const char *upload_data,
                          size_t *upload_data_size, 
                          void **con_cls) {
    if (*con_cls == NULL) {
        // First call for this request
        *con_cls = malloc(1);
        if (*con_cls == NULL) return NULL;
        return NULL;
    }

    if (*upload_data_size == 0) {
        // All data has been read
        free(*con_cls);
        *con_cls = NULL;
        return strdup("{}");  // Return empty JSON object if no data
    }

    // Copy the upload data
    char *post_data = malloc(*upload_data_size + 1);
    if (post_data == NULL) return NULL;
    
    memcpy(post_data, upload_data, *upload_data_size);
    post_data[*upload_data_size] = '\0';
    *upload_data_size = 0;  // Mark that we've processed the data
    
    return post_data;
}
