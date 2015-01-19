#ifndef TWBCTF_H
#define TWBCTF_H

// Libraries //
#include <stdbool.h>

typedef bool (* test_p) (void);

struct test {
    char * desc;
    test_p func;
};

uint8_t longest_desc(const struct test *list);

#endif
