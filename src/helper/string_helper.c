#include "string_helper.h"

#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

int parse_int(const char* str, int* out) {
    if (str == NULL || *str == '\0') return -1;// empty string or NULL

    char* end;
    errno = 0;
    const long val = strtol(str, &end, 10);

    if (end == str || *end != '\0') {
        return -1;// Error: not a valid integer
    }
    if ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE) {
        return -1;// overflow
    }
    if (val < INT_MIN || val > INT_MAX) {
        return -1;// does not fit in int
    }

    *out = (int) val;
    return 0;// Success
}

int parse_float(const char* str, float* out) {
    if (str == NULL || *str == '\0') return -1;// empty string or NULL

    char* end;
    const double val = strtod(str, &end);

    if (end == str || *end != '\0') {
        return -1;// Error: not a valid float
    }
    if ((val == HUGE_VAL || val == -HUGE_VAL) && errno == ERANGE) {
        return -1;// overflow
    }
    if (val < -FLT_MAX || val > FLT_MAX) {
        return -1;// does not fit in float
    }
    if (isnan(val) || isinf(val)) {
        return -1;//  not a number or infinity
    }

    *out = (float) val;
    return 0;// Success
}
