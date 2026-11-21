#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#include "config.h"
#include "http.h"
#include "input_output.h"
#include "util.h"

/*
    TODO:
    - Set socket SO_REUSEADDR option with setsockopt
    - Check, which functions actually need to communicate errors

    ROADMAP:
    - Improve memory management so that it's efficient and scaleable
    - Cache recent accessed files to a circular buffer or something
    - Moving to lower level apis e.g. for file handling because handrolling stuff is fun
*/

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
    EXIT_ERROR("^");
    }

    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    // htons converst from hostbyte to network byte order
    address.sin_port = htons(LISTEN_PORT);

    // inet_pton makes usable ip-address from a string
    int inet_pton_return = inet_pton(AF_INET, LISTEN_IP, &address.sin_addr);
    if (inet_pton_return == 0) {
        EXIT_ERROR("Invalid character string passed to inet_pton");
    }
    else if (inet_pton_return == -1) {
        perror("inet_pton");
        EXIT_ERROR("^");
    }

    if (bind(fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind");
        EXIT_ERROR("^");
    }

    int backlog = 4;
    int listen_return = listen(fd, backlog);
    if (listen_return < 0) {
        perror("listen");
        EXIT_ERROR("^");
    }

    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    char client_ip_address[16];

    for (;;) {
        int fd_accept = accept(fd, (struct sockaddr*)&client_address,
                &client_address_length);

        if (fd_accept < 0) {
            perror("accept");
            EXIT_ERROR("^");
        }

        if (inet_ntop(AF_INET, &client_address.sin_addr, client_ip_address,
                    client_address_length) == NULL) {
            perror("inet_ntop");
        }

        LOG_MESSAGE("New connection from %s:%d", client_ip_address,
                ntohs(client_address.sin_port));

        struct HttpRequest request;

        request_deserialize(&request, fd_accept);
        LOG_MESSAGE("Method: %d, Path: %s, Version: %s",
                request.method, request.path, request.version);
        
        struct HttpResponse response;
        int response_exit = http_response_create(&response, &request);
        if (response_exit < 0) {
            EXIT_ERROR("^");
        }

        response_serialize_and_send(&response);
        http_response_destroy(&response);

        close(fd_accept);
    }
    
    close(fd);

}
