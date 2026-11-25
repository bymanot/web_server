#pragma once

// Creates and binds the socket
int socket_initialize();

// On failure errno is set by stat and zero is returned
size_t get_file_size(const char*);

// Writes file content to dst buffer, zero returned on success, -1 on failure
int get_file_data(char*, size_t, const char*);

// Sends data to accepted connection (fd), zero returned on success, -1 on failure
int send_data(int, char*, size_t);
