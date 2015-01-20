/* yasl, Yet Another String Library for C
 *
 * Copyright (c) 2006-2014, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef YASL_H
#define YASL_H

#define YASL_MAX_PREALLOC (1024*1024)

#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>

typedef char *yastr;

struct yastrhdr {
	size_t len;
	size_t free;
	char buf[];
};


/**
 * User API function prototypes
 */

/// Initialization
yastr  yaslnew(const void *init, size_t initlen);
yastr  yasldup(const yastr s);
yastr  yaslempty(void);
yastr  yaslfromlonglong(long long value);


/// Querying
int    yaslcmp(const yastr s1, const yastr s2);


/// Modification
void   yaslclear(yastr s);
yastr  yaslgrowzero(yastr s, size_t len);
yastr  yaslcpylen(yastr s, const char *t, size_t len);
yastr  yaslcpy(yastr s, const char *t);
yastr  yasljoin(char **argv, int argc, char *sep, size_t seplen);
yastr  yasljoinyasl(yastr *argv, int argc, const char *sep, size_t seplen);
yastr  yaslmapchars(yastr s, const char *from, const char *to, size_t setlen);
void   yaslrange(yastr s, ptrdiff_t start, ptrdiff_t end);
void   yasltolower(yastr s);
void   yasltoupper(yastr s);
void   yasltrim(yastr s, const char *cset);
void   yaslupdatelen(yastr s);
yastr *yaslsplitargs(const char *line, int *argc);
yastr *yaslsplitlen(const char *s, size_t len, const char *sep, size_t seplen, size_t *count);

// Concatenation
yastr  yaslcat(yastr s, const char *t);
yastr  yaslcatyasl(yastr s, const yastr t);
yastr  yaslcatlen(yastr s, const void *t, size_t len);
yastr  yaslcatrepr(yastr s, const char *p, size_t len);
yastr  yaslcatvprintf(yastr s, const char *fmt, va_list ap);
#ifdef __GNUC__
yastr  yaslcatprintf(yastr s, const char *fmt, ...)
            __attribute__((format(printf, 2, 3)));
#else
yastr  yaslcatprintf(yastr s, const char *fmt, ...);
#endif


/// Freeing
void   yaslfree(yastr s);
void   yaslfreesplitres(yastr *tokens, size_t count);


/// Low-level API function prototypes
size_t yaslAllocSize(yastr s);
void   yaslIncrLen(yastr s, size_t incr);
yastr  yaslMakeRoomFor(yastr s, size_t addlen);
yastr  yaslRemoveFreeSpace(yastr s);


/// Low-level helper functions
int    is_hex_digit(char c);
int    hex_digit_to_int(char c);


/**
 * Inline functions
 */

static inline struct yastrhdr *yaslheader(const yastr s) {
	/* The yastrhdr pointer has a different alignment than the original char
	 * pointer, so cast it through a void pointer to silence the warning. */
	return (void *)(s - (sizeof (struct yastrhdr)));
}

static inline yastr yaslauto(const char *s) {
	return yaslnew(s, s ? strlen(s) : 0);
}

static inline size_t yaslavail(const yastr s) {
	return yaslheader(s)->free;
}

static inline size_t yasllen(const yastr s) {
	return yaslheader(s)->len;
}


#endif
