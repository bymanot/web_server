#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "config.h"
#include "util.h"

int socket_initialize() {
    
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
    if (listen(fd, backlog) < 0) {
        perror("listen");
        EXIT_ERROR("^");
    }

    return fd;
}

size_t get_file_size(const char* file_path) {
    struct stat file_stat;

    if (stat(file_path, &file_stat) < 0) {
        LOG_ERROR("stat failed to fetch data");
        return 0;
    }

    return (size_t)file_stat.st_size;
}

int get_file_data(char* dst, size_t size, const char* file_path) {
    FILE* stream = fopen(file_path, "r");
    if (stream == NULL) {
        LOG_ERROR("fopen failed to open file");
        return -1;
    }

    // fread doesn't need a loop
    size_t fread_ret = fread(dst, sizeof(char), size, stream);
    if (fread_ret < 1) {
        LOG_MESSAGE("Zero bytes read from file");
        if (ferror(stream)) {
            LOG_ERROR("File couldn't be read");
            fclose(stream);
            return -1;
        }
    }

    fclose(stream);
    return 0;
}

int send_data(int fd, char* message, size_t message_size) {
    size_t total_sent = 0;

    while (total_sent < message_size) {
        ssize_t send_return = send(fd, message + total_sent,
                message_size - total_sent, 0);

        if (send_return < 0) {
            return -1;
        }

        total_sent += send_return;
    }
    return 0;
}
