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

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sds.h"


/// Initialization

/* Create a new sds string with the content specified by the 'init' pointer
 * and 'initlen'.
 * If NULL is used for 'init' the string is initialized with zero bytes.
 *
 * The string is always null-termined (all the sds strings are, always) so
 * even if you create an sds string with:
 *
 * mystring = sdsnew("abc", 3");
 *
 * You can print the string with printf() as there is an implicit \0 at the
 * end of the string. However the string is binary safe and can contain
 * \0 characters in the middle, as the length is stored in the sds header. */
sds sdsnew(const void *init, size_t initlen) {
	struct sdshdr *sh;

	if (init) {
		sh = malloc(sizeof(struct sdshdr) + initlen + 1);
	} else {
		sh = calloc(sizeof(struct sdshdr) + initlen + 1, 1);
	}
	if (sh == NULL) { return NULL; }

	sh->len = initlen;
	sh->free = 0;
	if (initlen && init) {
		memcpy(sh->buf, init, initlen);
	}
	sh->buf[initlen] = '\0';
	return (char*)sh->buf;
}

/* Duplicate an sds string. */
sds sdsdup(const sds s) {
	return sdsnew(s, sdslen(s));
}

/* Create an empty (zero length) sds string. Even in this case the string
 * always has an implicit null term. */
sds sdsempty(void) {
	return sdsnew("", 0);
}

/* Create an sds string from a long long value. It is much faster than:
 *
 * sdscatprintf(sdsempty(), "%lld\n", value);
 */
sds sdsfromlonglong(long long value) {
	char buf[32], *p;
	unsigned long long v;

	v = (unsigned long long)((value < 0) ? -value : value);
	p = buf + 31; /* point to the last character */
	do {
		*p-- = '0' + (v%10);
		v /= 10;
	} while(v);
	if (value < 0) { *p-- = '-'; }
	p++;
	return sdsnew(p, (size_t)(32 - (p - buf)));
}


/// Querying

/* Compare two sds strings s1 and s2 with memcmp().
 *
 * Return value:
 *
 *     1 if s1 > s2.
 *    -1 if s1 < s2.
 *     0 if s1 and s2 are exactly the same binary string.
 *
 * If two strings share exactly the same prefix, but one of the two has
 * additional characters, the longer string is considered to be greater than
 * the smaller one. */
int sdscmp(const sds s1, const sds s2) {
	size_t l1, l2, minlen;
	int cmp;

	l1 = sdslen(s1);
	l2 = sdslen(s2);
	minlen = (l1 < l2) ? l1 : l2;
	cmp = memcmp(s1, s2, minlen);
	if (cmp == 0) { return (int)(l1 - l2); }
	return cmp;
}


/// Modification

/* Modify an sds string on-place to make it empty (zero length).
 * However all the existing buffer is not discarded but set as free space
 * so that next append operations will not require allocations up to the
 * number of bytes previously available. */
void sdsclear(sds s) {
	struct sdshdr *sh = sdsheader(s);
	sh->free += sh->len;
	sh->len = 0;
	sh->buf[0] = '\0';
}

/* Grow the sds to have the specified length. Bytes that were not part of
 * the original length of the sds will be set to zero.
 *
 * if the specified length is smaller than the current length, no operation
 * is performed. */
sds sdsgrowzero(sds s, size_t len) {
	struct sdshdr *sh = sdsheader(s);
	size_t totlen, curlen = sh->len;

	if (len <= curlen) { return s; }
	s = sdsMakeRoomFor(s, len - curlen);
	if (s == NULL) { return NULL; }

	/* Make sure added region doesn't contain garbage */
	sh = sdsheader(s);
	memset(s + curlen, 0, (len - curlen + 1)); /* also set trailing \0 byte */
	totlen = sh->len + sh->free;
	sh->len = len;
	sh->free = totlen - sh->len;
	return s;
}

/* Destructively modify the sds string 's' to hold the specified binary
 * safe string pointed by 't' of length 'len' bytes. */
sds sdscpylen(sds s, const char *t, size_t len) {
	struct sdshdr *sh = sdsheader(s);
	size_t totlen = sh->free + sh->len;

	if (totlen < len) {
		s = sdsMakeRoomFor(s, len - sh->len);
		if (s == NULL) { return NULL; }
		sh = sdsheader(s);
		totlen = sh->free + sh->len;
	}
	memcpy(s, t, len);
	s[len] = '\0';
	sh->len = len;
	sh->free = totlen - len;
	return s;
}

/* Like sdscpylen() but 't' must be a null-termined string so that the length
 * of the string is obtained with strlen(). */
sds sdscpy(sds s, const char *t) {
	return sdscpylen(s, t, strlen(t));
}

/* Join an array of C strings using the specified separator (also a C string).
 * Returns the result as an sds string. */
sds sdsjoin(char **argv, int argc, char *sep, size_t seplen) {
	sds join = sdsempty();

	for (int j = 0; j < argc; j++) {
		join = sdscat(join, argv[j]);
		if (j != argc - 1) { join = sdscatlen(join, sep, seplen); }
	}
	return join;
}

/* Like sdsjoin, but joins an array of SDS strings. */
sds sdsjoinsds(sds *argv, int argc, const char *sep, size_t seplen) {
	sds join = sdsempty();

	for (int j = 0; j < argc; j++) {
		join = sdscatsds(join, argv[j]);
		if (j != argc - 1) { join = sdscatlen(join, sep, seplen); }
	}
	return join;
}

/* Modify the string substituting all the occurrences of the set of
 * characters specified in the 'from' string to the corresponding character
 * in the 'to' array.
 *
 * For instance: sdsmapchars(mystring, "ho", "01", 2)
 * will have the effect of turning the string "hello" into "0ell1".
 *
 * The function returns the sds string pointer, that is always the same
 * as the input pointer since no resize is needed. */
sds sdsmapchars(sds s, const char *from, const char *to, size_t setlen) {
	for (size_t j = 0; j < sdslen(s); j++) {
		for (size_t i = 0; i < setlen; i++) {
			if (s[j] == from[i]) {
				s[j] = to[i];
				break;
			}
		}
	}
	return s;
}

/* Turn the string into a smaller (or equal) string containing only the
 * substring specified by the 'start' and 'end' indexes.
 *
 * start and end can be negative, where -1 means the last character of the
 * string, -2 the penultimate character, and so forth.
 *
 * The interval is inclusive, so the start and end characters will be part
 * of the resulting string.
 *
 * The string is modified in-place.
 *
 * Example:
 *
 * s = sdsauto("Hello World");
 * sdsrange(s, 1, -1); => "ello World"
 */
void sdsrange(sds s, ptrdiff_t start, ptrdiff_t end) {
	struct sdshdr *sh = sdsheader(s);
	size_t newlen, len = sdslen(s);

	if (len == 0) { return; }
	if (start < 0) {
		start = (ptrdiff_t)len + start;
		if (start < 0) { start = 0; }
	}
	if (end < 0) {
		end = (ptrdiff_t)len + end;
		if (end < 0) { end = 0; }
	}
	newlen = (size_t)((start > end) ? 0 : (end - start) + 1);
	if (newlen != 0) {
		if ((size_t)start >= len) {
			newlen = 0;
		} else if ((size_t)end >= len) {
			end = (ptrdiff_t)len - 1;
			newlen = (size_t)((start > end) ? 0 : (end - start) + 1);
		}
	} else {
		start = 0;
	}
	if (start && newlen) { memmove(sh->buf, sh->buf + start, newlen); }
	sh->buf[newlen] = 0;
	sh->free = sh->free + (sh->len - newlen);
	sh->len = newlen;
}

/* Apply tolower() to every character of the sds string 's'. */
void sdstolower(sds s) {
	for (size_t j = 0; j < sdslen(s); j++) {
		s[j] = (char)tolower(s[j]);
	}
}

/* Apply toupper() to every character of the sds string 's'. */
void sdstoupper(sds s) {
	for (size_t j = 0; j < sdslen(s); j++) {
		s[j] = (char)toupper(s[j]);
	}
}

/* Remove the part of the string from left and from right composed just of
 * contiguous characters found in 'cset', that is a null terminted C string.
 *
 * After the call, the modified sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = sdsauto("AA...AA.a.aa.aHelloWorld     :::");
 * s = sdstrim(s, "A. :");
 * printf("%s\n", s);
 *
 * Output will be just "Hello World".
 */
void sdstrim(sds s, const char *cset) {
	struct sdshdr *sh = sdsheader(s);
	char *start, *end, *sp, *ep;
	size_t len;

	sp = start = s;
	ep = end = s + sdslen(s) - 1;
	while(sp <= end && strchr(cset, *sp)) { sp++; }
	while(ep > start && strchr(cset, *ep)) { ep--; }
	len = (size_t)((sp > ep) ? 0 : ((ep - sp) + 1));
	if (sh->buf != sp) { memmove(sh->buf, sp, len); }
	sh->buf[len] = '\0';
	sh->free = sh->free + (sh->len - len);
	sh->len = len;
}

/* Set the sds string length to the length as obtained with strlen(), so
 * considering as content only up to the first null term character.
 *
 * This function is useful when the sds string is hacked manually in some
 * way, like in the following example:
 *
 * s = sdsauto("foobar");
 * s[2] = '\0';
 * sdsupdatelen(s);
 * printf("%d\n", sdslen(s));
 *
 * The output will be "2", but if we comment out the call to sdsupdatelen()
 * the output will be "6" as the string was modified but the logical length
 * remains 6 bytes. */
void sdsupdatelen(sds s) {
	struct sdshdr *sh = sdsheader(s);
	size_t reallen = strlen(s);
	sh->free += (sh->len - reallen);
	sh->len = reallen;
}

/* Split a line into arguments, where every argument can be in the
 * following programming-language REPL-alike form:
 *
 * foo bar "newline are supported\n" and "\xff\x00otherstuff"
 *
 * The number of arguments is stored into *argc, and an array
 * of sds is returned.
 *
 * The caller should free the resulting array of sds strings with
 * sdsfreesplitres().
 *
 * Note that sdscatrepr() is able to convert back a string into
 * a quoted string in the same format sdssplitargs() is able to parse.
 *
 * The function returns the allocated tokens on success, even when the
 * input string is empty, or NULL if the input contains unbalanced
 * quotes or closed quotes followed by non space characters
 * as in: "foo"bar or "foo'
 */
sds *sdssplitargs(const char *line, int *argc) {
	const char *p = line;
	char *current = NULL;
	char **vector = NULL;

	*argc = 0;
	while(1) {
		/* skip blanks */
		while(*p && isspace(*p)) { p++; }
		if (*p) {
			/* get a token */
			int inq=0;  /* set to 1 if we are in "quotes" */
			int insq=0; /* set to 1 if we are in 'single quotes' */
			int done=0;

			if (current == NULL) { current = sdsempty(); }
			while(!done) {
				if (inq) {
					if (*p == '\\' && *(p + 1) == 'x' &&
					                         is_hex_digit(*(p + 2)) &&
					                         is_hex_digit(*(p + 3)))
					{
						unsigned char byte;

						byte = (unsigned char)((hex_digit_to_int(*(p + 2)) * 16) +
						                        hex_digit_to_int(*(p + 3)));
						current = sdscatlen(current, (char*)&byte, 1);
						p += 3;
					} else if (*p == '\\' && *(p + 1)) {
						char c;

						p++;
						switch(*p) {
						case 'n': c = '\n'; break;
						case 'r': c = '\r'; break;
						case 't': c = '\t'; break;
						case 'b': c = '\b'; break;
						case 'a': c = '\a'; break;
						default: c = *p; break;
						}
						current = sdscatlen(current, &c, 1);
					} else if (*p == '"') {
						/* closing quote must be followed by a space or
						 * nothing at all. */
						if (*(p + 1) && !isspace(*(p + 1))) { goto err; }
						done=1;
					} else if (!*p) {
						/* unterminated quotes */
						goto err;
					} else {
						current = sdscatlen(current, p, 1);
					}
				} else if (insq) {
					if (*p == '\\' && *(p + 1) == '\'') {
						p++;
						current = sdscatlen(current, "'", 1);
					} else if (*p == '\'') {
						/* closing quote must be followed by a space or
						 * nothing at all. */
						if (*(p + 1) && !isspace(*(p + 1))) { goto err; }
						done=1;
					} else if (!*p) {
						/* unterminated quotes */
						goto err;
					} else {
						current = sdscatlen(current, p, 1);
					}
				} else {
					switch(*p) {
					case ' ':
					case '\n':
					case '\r':
					case '\t':
					case '\0':
						done=1;
						break;
					case '"':
						inq=1;
						break;
					case '\'':
						insq=1;
						break;
					default:
						current = sdscatlen(current, p, 1);
						break;
					}
				}
				if (*p) { p++; }
			}
			/* add the token to the vector */
			vector = realloc(vector, (unsigned long)((*argc) + 1) * (sizeof (char *)));
			vector[*argc] = current;
			(*argc)++;
			current = NULL;
		} else {
			/* Even on empty input string return something not NULL. */
			if (vector == NULL) { vector = malloc(sizeof(void*)); }
			return vector;
		}
	}

err:
	while((*argc)--) {
		sdsfree(vector[*argc]);
	}
	free(vector);
	if (current) { sdsfree(current); }
	*argc = 0;
	return NULL;
}

/* Split 's' with separator in 'sep'. An array
 * of sds strings is returned. *count will be set
 * by reference to the number of tokens returned.
 *
 * On out of memory, zero length string, zero length
 * separator, NULL is returned.
 *
 * Note that 'sep' is able to split a string using
 * a multi-character separator. For example
 * sdssplit("foo_-_bar", "_-_"); will return two
 * elements "foo" and "bar".
 *
 * This version of the function is binary-safe but
 * requires length arguments. sdssplit() is just the
 * same function but for zero-terminated strings.
 */
sds *sdssplitlen(const char *s, size_t len, const char *sep, size_t seplen, size_t *count) {
	size_t elements = 0, slots = 5, start = 0;
	sds *tokens;

	if (seplen < 1) { return NULL; }

	tokens = malloc(sizeof(sds)*slots);
	if (tokens == NULL) { return NULL; }

	if (len == 0) {
		*count = 0;
		return tokens;
	}
	for (size_t j = 0; j < (len - (seplen - 1)); j++) {
		/* make sure there is room for the next element and the final one */
		if (slots < elements + 2) {
			sds *newtokens;

			slots *= 2;
			newtokens = realloc(tokens, sizeof(sds) * slots);
			if (newtokens == NULL) { goto cleanup; }
			tokens = newtokens;
		}
		/* search the separator */
		if ((seplen == 1 && *(s + j) == sep[0]) || (memcmp(s + j, sep, seplen) == 0)) {
			tokens[elements] = sdsnew(s + start, (size_t)(j - start));
			if (tokens[elements] == NULL) { goto cleanup; }
			elements++;
			start = j + seplen;
			j = j + seplen - 1; /* skip the separator */
		}
	}
	/* Add the final element. We are sure there is room in the tokens array. */
	tokens[elements] = sdsnew(s + start, (size_t)(len - start));
	if (tokens[elements] == NULL) { goto cleanup; }
	elements++;
	*count = elements;
	return tokens;

cleanup:
	{
		for (size_t i = 0; i < elements; i++) sdsfree(tokens[i]);
		free(tokens);
		*count = 0;
		return NULL;
	}
}

// Concatenation

/* Append the specified null termianted C string to the sds string 's'.
 *
 * After the call, the passed sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
sds sdscat(sds s, const char *t) {
	return sdscatlen(s, t, strlen(t));
}

/* Append the specified sds 't' to the existing sds 's'.
 *
 * After the call, the modified sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
sds sdscatsds(sds s, const sds t) {
	return sdscatlen(s, t, sdslen(t));
}

/* Append the specified binary-safe string pointed by 't' of 'len' bytes to the
 * end of the specified sds string 's'.
 *
 * After the call, the passed sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
sds sdscatlen(sds s, const void *t, size_t len) {
	struct sdshdr *sh;
	size_t curlen = sdslen(s);

	s = sdsMakeRoomFor(s, len);
	if (s == NULL) { return NULL; }
	sh = sdsheader(s);
	memcpy(s + curlen, t, len);
	sh->len = curlen + len;
	sh->free = sh->free - len;
	s[curlen + len] = '\0';
	return s;
}

/* Append to the sds string "s" an escaped string representation where
 * all the non-printable characters (tested with isprint()) are turned into
 * escapes in the form "\n\r\a...." or "\x<hex-number>".
 *
 * After the call, the modified sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
sds sdscatrepr(sds s, const char *p, size_t len) {
	s = sdscatlen(s, "\"", 1);
	while(len--) {
		switch(*p) {
		case '\\':
		case '"':
			s = sdscatprintf(s, "\\%c", *p);
			break;
		case '\n': s = sdscatlen(s, "\\n", 2); break;
		case '\r': s = sdscatlen(s, "\\r", 2); break;
		case '\t': s = sdscatlen(s, "\\t", 2); break;
		case '\a': s = sdscatlen(s, "\\a", 2); break;
		case '\b': s = sdscatlen(s, "\\b", 2); break;
		default:
			if (isprint(*p)) {
				s = sdscatprintf(s, "%c", *p);
			} else {
				s = sdscatprintf(s, "\\x%02x", (unsigned char)*p);
				break;
			}
		}
		p++;
	}
	return sdscatlen(s, "\"", 1);
}

/* Like sdscatpritf() but gets va_list instead of being variadic. */
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
sds sdscatvprintf(sds s, const char *fmt, va_list ap) {
	va_list cpy;
	char *buf, *t;
	size_t buflen = 16;

	while(1) {
		buf = malloc(buflen);
		if (buf == NULL) { return NULL; }
		buf[buflen - 2] = '\0';
		va_copy(cpy, ap);
		vsnprintf(buf, buflen, fmt, cpy);
		if (buf[buflen - 2] != '\0') {
			free(buf);
			buflen *= 2;
			continue;
		}
		break;
	}
	t = sdscat(s, buf);
	free(buf);
	return t;
}
#pragma GCC diagnostic warning "-Wformat-nonliteral"

/* Append to the sds string 's' a string obtained using printf-alike format
 * specifier.
 *
 * After the call, the modified sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = sdsempty("Sum is: ");
 * s = sdscatprintf(s, "%d + %d = %d", a, b, a + b).
 *
 * Often you need to create a string from scratch with the printf-alike
 * format. When this is the need, just use sdsempty() as the target string:
 *
 * s = sdscatprintf(sdsempty(), "... your format ...", args);
 */
sds sdscatprintf(sds s, const char *fmt, ...) {
	va_list ap;
	char *t;
	va_start(ap, fmt);
	t = sdscatvprintf(s, fmt, ap);
	va_end(ap);
	return t;
}


/// Freeing

/* Free an sds string. No operation is performed if 's' is NULL. */
void sdsfree(sds s) {
	if (s == NULL) { return; }
	free(sdsheader(s));
}

/* Free the result returned by sdssplitlen(), or do nothing if 'tokens' is NULL. */
void sdsfreesplitres(sds *tokens, size_t count) {
	if (!tokens) { return; }
	while(count--) {
		sdsfree(tokens[count]);
	}
	free(tokens);
}


// Low-level functions

/* Return the total size of the allocation of the specifed sds string,
 * including:
 * 1) The sds header before the pointer.
 * 2) The string.
 * 3) The free buffer at the end if any.
 * 4) The implicit null term.
 */
size_t sdsAllocSize(sds s) {
	struct sdshdr *sh = sdsheader(s);

	return sizeof(*sh) + sh->len + sh->free + 1;
}

/* Increment the sds length and decrements the left free space at the
 * end of the string according to 'incr'. Also set the null term
 * in the new end of the string.
 *
 * This function is used in order to fix the string length after the
 * user calls sdsMakeRoomFor(), writes something after the end of
 * the current string, and finally needs to set the new length.
 *
 * Usage example:
 *
 * Using sdsIncrLen() and sdsMakeRoomFor() it is possible to mount the
 * following schema, to cat bytes coming from the kernel to the end of an
 * sds string without copying into an intermediate buffer:
 *
 * oldlen = sdslen(s);
 * s = sdsMakeRoomFor(s, BUFFER_SIZE);
 * nread = read(fd, s + oldlen, BUFFER_SIZE);
 * ... check for nread <= 0 and handle it ...
 * sdsIncrLen(s, nread);
 */
void sdsIncrLen(sds s, size_t incr) {
	struct sdshdr *sh = sdsheader(s);

	assert(sh->free >= incr);
	sh->len += incr;
	sh->free -= incr;
	s[sh->len] = '\0';
}

/* Enlarge the free space at the end of the sds string so that the caller
 * is sure that after calling this function can overwrite up to addlen
 * bytes after the end of the string, plus one more byte for nul term.
 *
 * Note: this does not change the *length* of the sds string as returned
 * by sdslen(), but only the free buffer space we have. */
sds sdsMakeRoomFor(sds s, size_t addlen) {
	struct sdshdr *sh, *newsh;
	size_t free = sdsavail(s);
	size_t len, newlen;

	if (free >= addlen) { return s; }
	len = sdslen(s);
	sh = sdsheader(s);
	newlen = (len + addlen);
	if (newlen < SDS_MAX_PREALLOC) {
		newlen *= 2;
	} else {
		newlen += SDS_MAX_PREALLOC;
	}
	newsh = realloc(sh, sizeof(struct sdshdr) + newlen + 1);
	if (newsh == NULL) { return NULL; }

	newsh->free = newlen - len;
	return newsh->buf;
}

/* Reallocate the sds string so that it has no free space at the end. The
 * contained string remains not altered, but next concatenation operations
 * will require a reallocation.
 *
 * After the call, the passed sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
sds sdsRemoveFreeSpace(sds s) {
	struct sdshdr *sh;

	sh = sdsheader(s);
	sh = realloc(sh, sizeof(struct sdshdr) + sh->len + 1);
	sh->free = 0;
	return sh->buf;
}


/// Low-level helper functions

/* Helper function for sdssplitargs() that returns non zero if 'c'
 * is a valid hex digit. */
int is_hex_digit(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
	       (c >= 'A' && c <= 'F');
}

/* Helper function for sdssplitargs() that converts a hex digit into an
 * integer from 0 to 15 */
int hex_digit_to_int(char c) {
	switch(c) {
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	case 'a': case 'A': return 10;
	case 'b': case 'B': return 11;
	case 'c': case 'C': return 12;
	case 'd': case 'D': return 13;
	case 'e': case 'E': return 14;
	case 'f': case 'F': return 15;
	default: return 0;
	}
}

