#include "handlers.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_LOG_LINE 512

// Function to log requests with client IP and timestamp
void log_request(struct MHD_Connection *connection,
                const char *method,
                const char *url,
                unsigned int status_code) {
    char timestamp[32];
    char client_ip[INET6_ADDRSTRLEN];
    const union MHD_ConnectionInfo *info;
    struct timeval tv;
    struct tm *tm_info;
    char log_line[MAX_LOG_LINE];
    
    // Get current timestamp
    gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    // Get client IP address
    info = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
    if (info && info->client_addr) {
        struct sockaddr *addr = (struct sockaddr *)info->client_addr;
        if (addr->sa_family == AF_INET) {
            struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
            inet_ntop(AF_INET, &(addr_in->sin_addr), client_ip, INET6_ADDRSTRLEN);
        } else if (addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)addr;
            inet_ntop(AF_INET6, &(addr_in6->sin6_addr), client_ip, INET6_ADDRSTRLEN);
        } else {
            strcpy(client_ip, "unknown");
        }
    } else {
        strcpy(client_ip, "unknown");
    }
    
    // Format and print log line
    snprintf(log_line, sizeof(log_line),
             "[%s] %s - %s %s %u",
             timestamp, client_ip, method, url, status_code);
    printf("%s\n", log_line);
    fflush(stdout);
}

enum MHD_Result send_error_response(struct MHD_Connection *connection,
                                  const char *method,
                                  const char *url,
                                  const char *error_msg,
                                  unsigned int status_code) {
    struct MHD_Response *response;
    enum MHD_Result ret;
    
    response = MHD_create_response_from_buffer(strlen(error_msg),
                                             (void*)error_msg,
                                             MHD_RESPMEM_MUST_COPY);
    if (response == NULL) return MHD_NO;
    
    ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    
    // Log the request after sending the response
    log_request(connection, method, url, status_code);
    
    return ret;
}

enum MHD_Result send_json_response(struct MHD_Connection *connection,
                                 const char *method,
                                 const char *url,
                                 const char *json_str,
                                 unsigned int status_code) {
    struct MHD_Response *response;
    enum MHD_Result ret;
    
    response = MHD_create_response_from_buffer(strlen(json_str),
                                             (void*)json_str,
                                             MHD_RESPMEM_MUST_COPY);
    if (response == NULL) return MHD_NO;
    
    MHD_add_response_header(response, "Content-Type", "application/json");
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, PATCH, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type");
    
    ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    
    // Log the request after sending the response
    log_request(connection, method, url, status_code);
    
    return ret;
}
