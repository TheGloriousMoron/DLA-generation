#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <lodepng/lodepng.h>
#include <stdarg.h>
#include <ctype.h>

#ifdef _WIN32 || _WIN64
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/types.h> // For ssize_t
#endif

#endif