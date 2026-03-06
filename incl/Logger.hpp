#pragma once

#include <stdio.h>

// taken from old C project, works fine in C++

#define LOG_INFO(fmt, ...)  \
    fprintf(stdout, "[INFO] [%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...)  \
    fprintf(stderr, "[WARN] [%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    fprintf(stderr, "[ERROR] [%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
    
