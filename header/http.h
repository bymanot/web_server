#pragma once

#include <stddef.h>

#define REQUEST_PATH_MAX        1024
#define REQUEST_VERSION_MAX     16

enum HttpStatusCode {
    CODE_200,
    CODE_400, // Bad request
    CODE_404,
    CODE_405 // Method not supported
};

enum HttpMethod {
    METHOD_GET,
    METHOD_UNSUPPORTED
};

struct HttpRequest {
    char path[REQUEST_PATH_MAX];
    char version[REQUEST_VERSION_MAX];
    enum HttpMethod method;
    int fd;
};

struct HttpResponse {
    size_t content_length;
    char* content;
    int fd;
    enum HttpStatusCode code;
};

// Extracts raw data from the socket to a HttpRequest struct
// Returns -1 on failure
int request_deserialize(struct HttpRequest*, int);

// Creates a response struct from the request
// Makes an allocation in the function for the response content
// Returns -1 on failure
int http_response_create(struct HttpResponse*, struct HttpRequest* input);

// Frees the allocated memory by the create function
void http_response_destroy(struct HttpResponse*);

// Formats data from the response struct and sends it to the socket
int response_serialize_and_send(struct HttpResponse*);

// Fills buffer with the respective string for the status code
// E.g. CODE_404 -> "404 Not Found"
void status_code_to_string(enum HttpStatusCode, char*);
