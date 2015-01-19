/* SDS (Simple Dynamic Strings), A C dynamic strings library.
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

#ifndef SDS_H
#define SDS_H

#define SDS_MAX_PREALLOC (1024*1024)

#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>

typedef char *sds;

struct sdshdr {
	size_t len;
	size_t free;
	char buf[];
};


/**
 * User API function prototypes
 */

/// Initialization
sds    sdsnew(const void *init, size_t initlen);
sds    sdsdup(const sds s);
sds    sdsempty(void);
sds    sdsfromlonglong(long long value);


/// Querying
int    sdscmp(const sds s1, const sds s2);


/// Modification
void   sdsclear(sds s);
sds    sdsgrowzero(sds s, size_t len);
sds    sdscpylen(sds s, const char *t, size_t len);
sds    sdscpy(sds s, const char *t);
sds    sdsjoin(char **argv, int argc, char *sep, size_t seplen);
sds    sdsjoinsds(sds *argv, int argc, const char *sep, size_t seplen);
sds    sdsmapchars(sds s, const char *from, const char *to, size_t setlen);
void   sdsrange(sds s, ptrdiff_t start, ptrdiff_t end);
void   sdstolower(sds s);
void   sdstoupper(sds s);
void   sdstrim(sds s, const char *cset);
void   sdsupdatelen(sds s);
sds   *sdssplitargs(const char *line, int *argc);
sds   *sdssplitlen(const char *s, size_t len, const char *sep, size_t seplen, size_t *count);

// Concatenation
sds    sdscat(sds s, const char *t);
sds    sdscatsds(sds s, const sds t);
sds    sdscatlen(sds s, const void *t, size_t len);
sds    sdscatrepr(sds s, const char *p, size_t len);
sds    sdscatvprintf(sds s, const char *fmt, va_list ap);
#ifdef __GNUC__
sds    sdscatprintf(sds s, const char *fmt, ...)
            __attribute__((format(printf, 2, 3)));
#else
sds    sdscatprintf(sds s, const char *fmt, ...);
#endif


/// Freeing
void   sdsfree(sds s);
void   sdsfreesplitres(sds *tokens, size_t count);


/// Low-level API function prototypes
size_t sdsAllocSize(sds s);
void   sdsIncrLen(sds s, size_t incr);
sds    sdsMakeRoomFor(sds s, size_t addlen);
sds    sdsRemoveFreeSpace(sds s);


/// Low-level helper functions
int    is_hex_digit(char c);
int    hex_digit_to_int(char c);


/**
 * Inline functions
 */

static inline struct sdshdr *sdsheader(const sds s) {
	/* The sdshdr pointer has a different alignment than the original char
	 * pointer, so cast it through a void pointer to silence the warning. */
	return (void *)(s - (sizeof (struct sdshdr)));
}

static inline sds sdsauto(const char *s) {
	return sdsnew(s, s ? strlen(s) : 0);
}

static inline size_t sdsavail(const sds s) {
	return sdsheader(s)->free;
}

static inline size_t sdslen(const sds s) {
	return sdsheader(s)->len;
}


#endif
