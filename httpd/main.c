#include <microhttpd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <json-c/json.h>
#include "handlers.h"

#define STATIC_DIR "/tmp/tuya/www"
#define MAX_PATH_LEN 1024

static const char* get_file_extension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

static const char* get_content_type(const char* ext) {
    if (strcmp(ext, "html") == 0) return "text/html";
    if (strcmp(ext, "js") == 0) return "application/javascript";
    if (strcmp(ext, "css") == 0) return "text/css";
    if (strcmp(ext, "png") == 0) return "image/png";
    if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, "gif") == 0) return "image/gif";
    return "application/octet-stream";
}

static enum MHD_Result serve_file(struct MHD_Connection *connection, const char *url, const char *method) {
    char filepath[MAX_PATH_LEN];
    struct MHD_Response *response;
    enum MHD_Result ret;
    int fd;
    struct stat st;
    
    // Remove leading slash and handle empty path
    const char *clean_url = (*url == '/') ? url + 1 : url;
    if (clean_url[0] == '\0') {
        clean_url = "index.html";
    }
    snprintf(filepath, sizeof(filepath), "%s/%s", STATIC_DIR, clean_url);
    
    // Check if file exists and is regular file
    if (stat(filepath, &st) == -1 || !S_ISREG(st.st_mode)) {
        return send_error_response(connection, method, url, "File not found", MHD_HTTP_NOT_FOUND);
    }
    
    // Open file
    fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        return send_error_response(connection, method, url, "Failed to open file", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }
    
    response = MHD_create_response_from_fd(st.st_size, fd);
    if (response == NULL) {
        close(fd);
        return send_error_response(connection, method, url, "Failed to create response", MHD_HTTP_INTERNAL_SERVER_ERROR);
    }
    
    const char *ext = get_file_extension(filepath);
    MHD_add_response_header(response, "Content-Type", get_content_type(ext));
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    
    // Log successful file serving
    log_request(connection, method, url, MHD_HTTP_OK);
    return ret;
}

static enum MHD_Result request_handler(void *cls, struct MHD_Connection *connection,
                                     const char *url, const char *method,
                                     const char *version, const char *upload_data,
                                     size_t *upload_data_size, void **con_cls) {
    // Handle API endpoints
    if (strncmp(url, "/api/", 5) == 0) {
        if (strcmp(url, "/api/gateway/status") == 0 && strcmp(method, "GET") == 0) {
            return handle_gateway_status(connection);
        }
        
        if (strcmp(url, "/api/logs") == 0 && strcmp(method, "GET") == 0) {
            enum MHD_Result ret = handle_logs(connection);
            if (ret == MHD_NO) {
                return send_error_response(connection, method, url, 
                                        "Internal server error", MHD_HTTP_INTERNAL_SERVER_ERROR);
            }
            return ret;
        }
        
        if (strcmp(url, "/api/settings") == 0) {
            if (strcmp(method, "GET") == 0) {
                enum MHD_Result ret = handle_settings_get(connection);
                if (ret == MHD_NO) {
                    return send_error_response(connection, method, url, 
                                            "Internal server error", MHD_HTTP_INTERNAL_SERVER_ERROR);
                }
                return ret;
            }
            if (strcmp(method, "PATCH") == 0) {
                enum MHD_Result ret = handle_settings_patch(connection, upload_data, upload_data_size, con_cls);
                if (ret == MHD_NO) {
                    return send_error_response(connection, method, url, 
                                            "Internal server error", MHD_HTTP_INTERNAL_SERVER_ERROR);
                }
                return ret;
            }
            // Method not allowed
            return send_error_response(connection, method, url, "Method not allowed", MHD_HTTP_METHOD_NOT_ALLOWED);
        }
        
        // API endpoint not found
        return send_error_response(connection, method, url, "API endpoint not found", MHD_HTTP_NOT_FOUND);
    }
    
    // Check if file exists at the requested path
    char filepath[MAX_PATH_LEN];
    struct stat st;
    snprintf(filepath, sizeof(filepath), "%s/%s", STATIC_DIR, (*url == '/') ? url + 1 : url);
    
    if (stat(filepath, &st) != -1 && S_ISREG(st.st_mode)) {
        // File exists, serve it
        return serve_file(connection, url, method);
    }
    
    // For non-API paths that don't exist, try serving index.html
    if (strncmp(url, "/api/", 5) != 0) {
        snprintf(filepath, sizeof(filepath), "%s/index.html", STATIC_DIR);
        if (stat(filepath, &st) != -1 && S_ISREG(st.st_mode)) {
            return serve_file(connection, "/index.html", method);
        }
    }
    
    // If we get here, neither the file nor index.html exists
    return send_error_response(connection, method, url, "File not found", MHD_HTTP_NOT_FOUND);
}

int main(void) {
    struct MHD_Daemon *daemon;
    
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, 80, NULL, NULL,
                            &request_handler, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;
    
    // Keep the main thread running
    while (1) {
        sleep(1);
    }
    
    MHD_stop_daemon(daemon);
    return 0;
}
