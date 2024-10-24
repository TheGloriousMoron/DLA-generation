#ifndef INCLUDES_H
#define INCLUDES_H

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#include <lodepng/lodepng.h>
#include <yaml.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#else
    #include <unistd.h>
    #include <limits.h> // For PATH_MAX
    #include <sys/types.h> // For ssize_t
#endif

#endif