#ifndef TWBCTF_H
#define TWBCTF_H

// Libraries //
#include <stdbool.h> // bool keyword

typedef bool (* test_p) (void);

struct test {
    const char * desc;
    test_p func;
};

extern const struct test test_list [];

// vim: set ts=4 sw=4 et:
#endif // TWBCTF_H
