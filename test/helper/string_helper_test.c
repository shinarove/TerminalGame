# include "../../src/helper/string_helper.h"

#include <stdio.h>
#include <assert.h>

void parse_int_test() {
    int result;
    int out;

    // Test: =0
    result = parse_int("0", &out);
    assert(result == 0);
    assert(out == 0);

    // Test: Valid integer
    result = parse_int("123", &out);
    assert(result == 0);
    assert(out == 123);

    // Test: Negative integer
    result = parse_int("-456", &out);
    assert(result == 0);
    assert(out == -456);

    // Test: Integer overflow
    result = parse_int("2147483648", &out); // INT_MAX + 1
    assert(result == -1);

    // Test: Integer underflow
    result = parse_int("-2147483649", &out); // INT_MIN - 1
    assert(result == -1);

    // Test: Invalid input (non-numeric)
    result = parse_int("abc", &out);
    assert(result == -1);

    // Test: Empty string
    result = parse_int("", &out);
    assert(result == -1);

    // Test: NULL input
    result = parse_int(NULL, &out);
    assert(result == -1);
}

void parse_float_test() {
    int result;
    float out;

    // Test: =0
    result = parse_float("0.0", &out);
    assert(result == 0);
    assert(out == 0.0);

    // Test: Valid float
    result = parse_float("123.45", &out);
    assert(result == 0);
    assert(out == 123.45f);

    // Test: Negative float
    result = parse_float("-678.90", &out);
    assert(result == 0);
    assert(out == -678.90f);

    // Test: Float overflow
    result = parse_float("1e40", &out); // Exceeds FLT_MAX
    assert(result == -1);

    // Test: Float underflow
    result = parse_float("-1e40", &out); // Exceeds -FLT_MAX
    assert(result == -1);

    // Test: Invalid input (non-numeric)
    result = parse_float("xyz", &out);
    assert(result == -1);

    // Test: Empty string
    result = parse_float("", &out);
    assert(result == -1);

    // Test: NULL input
    result = parse_float(NULL, &out);
    assert(result == -1);
}

int main(int argc, char* argv[]) {
    parse_int_test();
    parse_float_test();
    return 0;
}
