#pragma once
#include <stdio.h>
#include <stdlib.h>

#define KiB(x) 1024*x
#define MiB(x) 1024*1024*x
#define GiB(x) 1024*1024*1024*x

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOG_MESSAGE(msg, ...) \
        fprintf(stdout, ANSI_COLOR_YELLOW"[LOG]"ANSI_COLOR_RESET"\t%s:%d\t"msg"\n", \
                __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_ERROR(msg, ...) \
        fprintf(stderr, ANSI_COLOR_RED"[ERROR]"ANSI_COLOR_RESET"\t%s:%d\t"msg"\n", \
                __FILE__, __LINE__, ##__VA_ARGS__)

#define EXIT_ERROR(msg, ...) \
        fprintf(stderr, ANSI_COLOR_RED"[ABORT]"ANSI_COLOR_RESET"\t%s:%d\t"msg"\n", \
                __FILE__, __LINE__, ##__VA_ARGS__); \
        exit(1)
