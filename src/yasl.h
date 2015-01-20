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
sds    yaslnew(const void *init, size_t initlen);
sds    yasldup(const sds s);
sds    yaslempty(void);
sds    yaslfromlonglong(long long value);


/// Querying
int    yaslcmp(const sds s1, const sds s2);


/// Modification
void   yaslclear(sds s);
sds    yaslgrowzero(sds s, size_t len);
sds    yaslcpylen(sds s, const char *t, size_t len);
sds    yaslcpy(sds s, const char *t);
sds    yasljoin(char **argv, int argc, char *sep, size_t seplen);
sds    yasljoinyasl(sds *argv, int argc, const char *sep, size_t seplen);
sds    yaslmapchars(sds s, const char *from, const char *to, size_t setlen);
void   yaslrange(sds s, ptrdiff_t start, ptrdiff_t end);
void   yasltolower(sds s);
void   yasltoupper(sds s);
void   yasltrim(sds s, const char *cset);
void   yaslupdatelen(sds s);
sds   *yaslsplitargs(const char *line, int *argc);
sds   *yaslsplitlen(const char *s, size_t len, const char *sep, size_t seplen, size_t *count);

// Concatenation
sds    yaslcat(sds s, const char *t);
sds    yaslcatyasl(sds s, const sds t);
sds    yaslcatlen(sds s, const void *t, size_t len);
sds    yaslcatrepr(sds s, const char *p, size_t len);
sds    yaslcatvprintf(sds s, const char *fmt, va_list ap);
#ifdef __GNUC__
sds    yaslcatprintf(sds s, const char *fmt, ...)
            __attribute__((format(printf, 2, 3)));
#else
sds    yaslcatprintf(sds s, const char *fmt, ...);
#endif


/// Freeing
void   yaslfree(sds s);
void   yaslfreesplitres(sds *tokens, size_t count);


/// Low-level API function prototypes
size_t yaslAllocSize(sds s);
void   yaslIncrLen(sds s, size_t incr);
sds    yaslMakeRoomFor(sds s, size_t addlen);
sds    yaslRemoveFreeSpace(sds s);


/// Low-level helper functions
int    is_hex_digit(char c);
int    hex_digit_to_int(char c);


/**
 * Inline functions
 */

static inline struct sdshdr *yaslheader(const sds s) {
	/* The sdshdr pointer has a different alignment than the original char
	 * pointer, so cast it through a void pointer to silence the warning. */
	return (void *)(s - (sizeof (struct sdshdr)));
}

static inline sds yaslauto(const char *s) {
	return yaslnew(s, s ? strlen(s) : 0);
}

static inline size_t yaslavail(const sds s) {
	return yaslheader(s)->free;
}

static inline size_t yasllen(const sds s) {
	return yaslheader(s)->len;
}


#endif
