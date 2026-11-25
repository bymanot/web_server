#include <string.h>
#include <sys/socket.h>

#include "config.h"
#include "http.h"
#include "input_output.h"
#include "util.h"

int request_deserialize(struct HttpRequest* request, int fd_request) {
    request->fd = fd_request;

    size_t buffer_capacity = KiB(4);
    char buffer[buffer_capacity];
    memset(buffer, 0, buffer_capacity);
    size_t buffer_size = buffer_capacity - 1;

    size_t total_received = 0;
    ssize_t received      = 0;

    do {
        if (total_received >= buffer_size) {
            LOG_ERROR("Message too long, truncating...");
            break;
        }

        received = recv(fd_request, buffer + total_received,
                buffer_size - total_received, 0);
        if (received < 0) {
            LOG_ERROR("Couldn't read socket: ");
            perror("recv");
            return -1;
        }

        if (received == 0) {
            LOG_ERROR("Peer closed before request was complete");
            return -1;
        }

        total_received += received;
    } while (strstr(buffer, "\r\n\r\n") == NULL);

    buffer[total_received] = '\0';

    // TODO: Better solution for extracting substrings
    // This modifies the buffer and makes it invalid for later operations
    char* http_method  = strtok(buffer, " ");
    char* path         = strtok(NULL, " ");
    char* http_version = strtok(NULL, "\r");

    request->method = strcmp(http_method, "GET") == 0 ? METHOD_GET
            : METHOD_UNSUPPORTED;
    if (strlen(path) + 1 > REQUEST_PATH_MAX) {
        LOG_ERROR("Path string exceeds the buffer size");
        return -1;
    }
    strcpy(request->path, path);

    if (strlen(http_version) + 1 > REQUEST_VERSION_MAX) {
        LOG_ERROR("Version string exceeds the buffer size");
        return -1;
    }
    strcpy(request->version, http_version);

    return 0;
}

int http_response_create(struct HttpResponse* response, struct HttpRequest* request) {
    response->fd = request->fd;
    response->content = NULL;
    response->content_length = 0;
    if (request->method == METHOD_UNSUPPORTED) {
        response->code = CODE_405;
        return 0;
    }

    char path_buffer[128];
    if (file_path_from_url(request->path, path_buffer) < 0) {
        response->code = CODE_404;
        return 0;
    }

    response->code = CODE_200;

    size_t file_size = get_file_size(path_buffer);
    if (file_size == 0) {
        LOG_MESSAGE("File is empty");
        return 0;
    }

    response->content_length = file_size;
    response->content = malloc(file_size);
    if (response->content == NULL) {
        LOG_ERROR("Allocation failed");
        perror("malloc");
        return -1;
    }

    if (get_file_data(response->content,
            response->content_length, path_buffer) < 0) {
        free(response->content);
        return -1;
    }

    return 0;
}

void http_response_destroy(struct HttpResponse* response) {
    if (response->content == NULL) {
        return;
    }
    free(response->content);
}

int response_serialize_and_send(struct HttpResponse* response) {
    char header[KiB(2)];

    char code_buffer[36];
    status_code_to_string(response->code, code_buffer);

    int header_length = snprintf(
        header,
        sizeof(header),
        "HTTP/1.1 %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        code_buffer,
        response->content_length
    );

    if (send_data(response->fd, header, header_length) < 0) {
        return -1;
    }

    if (response->content_length) {
        if (send_data(response->fd, response->content,
                response->content_length) < 0) {
            return -1;
        }
    }

    return 0;
}

void status_code_to_string(enum HttpStatusCode status, char* buffer) {
    switch (status) {
    case CODE_200:
        strcpy(buffer, "200 OK");
        break;
    case CODE_400:
        strcpy(buffer, "400 Bad Request");
        break;
    case CODE_404:
        strcpy(buffer, "404 Not Found");
        break;
    case CODE_405:
        strcpy(buffer, "405 Method Not Allowed");
        break;
    }
}
