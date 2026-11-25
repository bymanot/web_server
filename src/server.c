#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#include "http.h"
#include "util.h"

int socket_initialize();

int main() {

    int fd = socket_initialize();

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

