#ifndef TWBCTF_H
#define TWBCTF_H

// Libraries //
#include <stdint.h>
#include <stdio.h>   // printf(), putchar()
#include <string.h>  // strlen()

typedef signed (* test_p) (void);

#define declare_test(x) signed x (void);\
                        signed x (void)

struct test {
    char * desc;
    test_p func;
};

extern const struct test test_list [];

// vim: set ts=4 sw=4 et:
#endif // TWBCTF_H
