#include "string_helper.h"

#include <stdlib.h>

int parse_int(const char* str, int* out) {
    char* end;
    const long val = strtol(str, &end, 10);

    if (*end != '\0' || end == str) {
        return -1;// Error: not a valid integer
    }
    *out = (int) val;
    return 0;// Success
}

int parse_float(const char* str, float* out) {
    char* end;
    const double val = strtod(str, &end);

    if (*end != '\0' || end == str) {
        return -1;// Error: not a valid float
    }
    *out = (float) val;
    return 0;// Success
}
