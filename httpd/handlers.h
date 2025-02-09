#ifndef HANDLERS_H
#define HANDLERS_H

#include <microhttpd.h>
#include <json-c/json.h>

// Gateway status handler
enum MHD_Result handle_gateway_status(struct MHD_Connection *connection);

// Logs handler
enum MHD_Result handle_logs(struct MHD_Connection *connection);

// Settings handlers
enum MHD_Result handle_settings_get(struct MHD_Connection *connection);
enum MHD_Result handle_settings_patch(struct MHD_Connection *connection,
                                    const char *upload_data,
                                    size_t *upload_data_size,
                                    void **con_cls);

// Helper functions
void log_request(struct MHD_Connection *connection,
                const char *method,
                const char *url,
                unsigned int status_code);

enum MHD_Result send_json_response(struct MHD_Connection *connection,
                                 const char *method,
                                 const char *url,
                                 const char *json_str,
                                 unsigned int status_code);

// Send error response with logging
enum MHD_Result send_error_response(struct MHD_Connection *connection,
                                  const char *method,
                                  const char *url,
                                  const char *error_msg,
                                  unsigned int status_code);

#endif // HANDLERS_H
