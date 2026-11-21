#include <string.h>
#include "http.h"

int file_path_from_url(char* url, char* dest) {
    if (strcmp(url, "/home") == 0) {
        strcpy(dest, "html/response.html");
    }
    else if (strcmp(url, "/") == 0) {
        strcpy(dest, "html/response.html");
    }
    else if (strcmp(url, "/test") == 0) {
        strcpy(dest, "html/response_test.html");
    }
    else if (strcmp(url, "/error") == 0) {
        strcpy(dest, "no_file_found");
    }
    else if (strcmp(url, "/post") == 0) {
        strcpy(dest, "html/post.html");
    }
    else {
        return -1;
    }
    return 0;
}

