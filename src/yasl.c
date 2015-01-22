/* yasl, Yet Another String Library for C
 *
 * Copyright (c) 2006-2014, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2014-2015, The yasl developers
 *
 * This file is under the 2-clause BSD license. See the LICENSE file for the
 * full license text
 */

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yasl.h"


// Initialization //

/* Create a new yasl string with the content specified by the 'init' pointer
 * and 'initlen'.
 * If NULL is used for 'init' the string is initialized with zero bytes.
 *
 * The string is always null-termined (all the yasl strings are, always) so
 * even if you create a yasl string with:
 *
 * mystring = yaslnew("abc", 3");
 *
 * You can print the string with printf() as there is an implicit \0 at the
 * end of the string. However the string is binary safe and can contain
 * \0 characters in the middle, as the length is stored in the yasl header. */
yastr
yaslnew(const void * init, size_t initlen) {
	struct yastrhdr * sh;

	if (init) {
		sh = malloc(sizeof(struct yastrhdr) + initlen + 1);
	} else {
		sh = calloc(sizeof(struct yastrhdr) + initlen + 1, 1);
	}
	if (!sh) { return NULL; }

	sh->len = initlen;
	sh->free = 0;
	if (initlen && init) {
		memcpy(sh->buf, init, initlen);
	}
	sh->buf[initlen] = '\0';
	return (char*)sh->buf;
}

/* Duplicate a yasl string. */
yastr
yasldup(const yastr s) {
	return yaslnew(s, yasllen(s));
}

/* Create an empty (zero length) yasl string. Even in this case the string
 * always has an implicit null term. */
yastr
yaslempty(void) {
	return yaslnew("", 0);
}

/* Create a yasl string from a long long value. It is much faster than:
 *
 * yaslcatprintf(yaslempty(), "%lld\n", value);
 */
yastr
yaslfromlonglong(long long value) {
	char buf[32], * p;
	unsigned long long v;

	v = (unsigned long long)((value < 0) ? -value : value);
	p = buf + 31; /* point to the last character */
	do {
		*p-- = '0' + (v%10);
		v /= 10;
	} while(v);
	if (value < 0) { *p-- = '-'; }
	p++;
	return yaslnew(p, (size_t)(32 - (p - buf)));
}


// Querying //

/* Compare two yasl strings s1 and s2 with memcmp().
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
int
yaslcmp(const yastr s1, const yastr s2) {
	size_t l1, l2, minlen;
	int cmp;

	l1 = yasllen(s1);
	l2 = yasllen(s2);
	minlen = (l1 < l2) ? l1 : l2;
	cmp = memcmp(s1, s2, minlen);
	if (cmp == 0) { return (int)(l1 - l2); }
	return cmp;
}


// Modification //

/* Modify a yasl string in-place to make it empty (zero length).
 * However all the existing buffer is not discarded but set as free space
 * so that next append operations will not require allocations up to the
 * number of bytes previously available. */
void
yaslclear(yastr s) {
	struct yastrhdr * sh = yaslheader(s);
	sh->free += sh->len;
	sh->len = 0;
	sh->buf[0] = '\0';
}

/* Grow the yasl string to have the specified length. Bytes that were not part
 * of the original length of the yasl string will be set to zero.
 *
 * if the specified length is smaller than the current length, no operation
 * is performed. */
yastr
yaslgrowzero(yastr s, size_t len) {
	struct yastrhdr * sh = yaslheader(s);
	size_t totlen, curlen = sh->len;

	if (len <= curlen) { return s; }
	s = yaslMakeRoomFor(s, len - curlen);
	if (!s) { return NULL; }

	/* Make sure added region doesn't contain garbage */
	sh = yaslheader(s);
	memset(s + curlen, 0, (len - curlen + 1)); /* also set trailing \0 byte */
	totlen = sh->len + sh->free;
	sh->len = len;
	sh->free = totlen - sh->len;
	return s;
}

/* Destructively modify the yasl string 's' to hold the specified binary
 * safe string pointed by 't' of length 'len' bytes. */
yastr
yaslcpylen(yastr s, const char * t, size_t len) {
	struct yastrhdr * sh = yaslheader(s);
	size_t totlen = sh->free + sh->len;

	if (totlen < len) {
		s = yaslMakeRoomFor(s, len - sh->len);
		if (!s) { return NULL; }
		sh = yaslheader(s);
		totlen = sh->free + sh->len;
	}
	memcpy(s, t, len);
	s[len] = '\0';
	sh->len = len;
	sh->free = totlen - len;
	return s;
}

/* Like yaslcpylen() but 't' must be a null-termined string so that the length
 * of the string is obtained with strlen(). */
yastr
yaslcpy(yastr s, const char * t) {
	return yaslcpylen(s, t, strlen(t));
}

/* Join an array of C strings using the specified separator (also a C string).
 * Returns the result as a yasl string. */
yastr
yasljoin(char ** argv, int argc, char * sep, size_t seplen) {
	yastr join = yaslempty();

	for (int j = 0; j < argc; j++) {
		join = yaslcat(join, argv[j]);
		if (j != argc - 1) { join = yaslcatlen(join, sep, seplen); }
	}
	return join;
}

/* Like yasljoin, but joins an array of yasl strings. */
yastr
yasljoinyasl(yastr * argv, int argc, const char * sep, size_t seplen) {
	yastr join = yaslempty();

	for (int j = 0; j < argc; j++) {
		join = yaslcatyasl(join, argv[j]);
		if (j != argc - 1) { join = yaslcatlen(join, sep, seplen); }
	}
	return join;
}

/* Modify the string substituting all the occurrences of the set of
 * characters specified in the 'from' string to the corresponding character
 * in the 'to' array.
 *
 * For instance: yaslmapchars(mystring, "ho", "01", 2)
 * will have the effect of turning the string "hello" into "0ell1".
 *
 * The function returns the yasl string pointer, that is always the same
 * as the input pointer since no resize is needed. */
yastr
yaslmapchars(yastr s, const char * from, const char * to, size_t setlen) {
	for (size_t j = 0; j < yasllen(s); j++) {
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
 * s = yaslauto("Hello World");
 * yaslrange(s, 1, -1); => "ello World"
 */
void
yaslrange(yastr s, ptrdiff_t start, ptrdiff_t end) {
	struct yastrhdr * sh = yaslheader(s);
	size_t newlen, len = yasllen(s);

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

/* Apply tolower() to every character of the yasl string 's'. */
void
yasltolower(yastr s) {
	for (size_t j = 0; j < yasllen(s); j++) {
		s[j] = (char)tolower(s[j]);
	}
}

/* Apply toupper() to every character of the yasl string 's'. */
void
yasltoupper(yastr s) {
	for (size_t j = 0; j < yasllen(s); j++) {
		s[j] = (char)toupper(s[j]);
	}
}

/* Remove the part of the string from left and from right composed just of
 * contiguous characters found in 'cset', that is a null terminted C string.
 *
 * After the call, the modified yasl string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = yaslauto("AA...AA.a.aa.aHelloWorld     :::");
 * s = yasltrim(s, "A. :");
 * printf("%s\n", s);
 *
 * Output will be just "Hello World".
 */
void
yasltrim(yastr s, const char * cset) {
	struct yastrhdr * sh = yaslheader(s);
	char * start, * end, * sp, * ep;
	size_t len;

	sp = start = s;
	ep = end = s + yasllen(s) - 1;
	while(sp <= end && strchr(cset, *sp)) { sp++; }
	while(ep > start && strchr(cset, *ep)) { ep--; }
	len = (size_t)((sp > ep) ? 0 : ((ep - sp) + 1));
	if (sh->buf != sp) { memmove(sh->buf, sp, len); }
	sh->buf[len] = '\0';
	sh->free = sh->free + (sh->len - len);
	sh->len = len;
}

/* Set the yasl string length to the length as obtained with strlen(), so
 * considering as content only up to the first null term character.
 *
 * This function is useful when the yasl string is hacked manually in some
 * way, like in the following example:
 *
 * s = yaslauto("foobar");
 * s[2] = '\0';
 * yaslupdatelen(s);
 * printf("%d\n", yasllen(s));
 *
 * The output will be "2", but if we comment out the call to yaslupdatelen()
 * the output will be "6" as the string was modified but the logical length
 * remains 6 bytes. */
void
yaslupdatelen(yastr s) {
	struct yastrhdr * sh = yaslheader(s);
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
 * of yasl strings is returned.
 *
 * The caller should free the resulting array of yasl strings with
 * yaslfreesplitres().
 *
 * Note that yaslcatrepr() is able to convert back a string into
 * a quoted string in the same format yaslsplitargs() is able to parse.
 *
 * The function returns the allocated tokens on success, even when the
 * input string is empty, or NULL if the input contains unbalanced
 * quotes or closed quotes followed by non space characters
 * as in: "foo"bar or "foo'
 */
yastr *
yaslsplitargs(const char * line, int * argc) {
	const char * p = line;
	char * current = NULL;
	char ** vector = NULL;

	*argc = 0;
	while(1) {
		/* skip blanks */
		while(*p && isspace(*p)) { p++; }
		if (*p) {
			/* get a token */
			int inq=0;  /* set to 1 if we are in "quotes" */
			int insq=0; /* set to 1 if we are in 'single quotes' */
			int done=0;

			if (!current) { current = yaslempty(); }
			while(!done) {
				if (inq) {
					if (*p == '\\' && *(p + 1) == 'x' &&
					                         is_hex_digit(*(p + 2)) &&
					                         is_hex_digit(*(p + 3)))
					{
						unsigned char byte;

						byte = (unsigned char)((hex_digit_to_int(*(p + 2)) * 16) +
						                        hex_digit_to_int(*(p + 3)));
						current = yaslcatlen(current, (char*)&byte, 1);
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
						current = yaslcatlen(current, &c, 1);
					} else if (*p == '"') {
						/* closing quote must be followed by a space or
						 * nothing at all. */
						if (*(p + 1) && !isspace(*(p + 1))) { goto err; }
						done=1;
					} else if (!*p) {
						/* unterminated quotes */
						goto err;
					} else {
						current = yaslcatlen(current, p, 1);
					}
				} else if (insq) {
					if (*p == '\\' && *(p + 1) == '\'') {
						p++;
						current = yaslcatlen(current, "'", 1);
					} else if (*p == '\'') {
						/* closing quote must be followed by a space or
						 * nothing at all. */
						if (*(p + 1) && !isspace(*(p + 1))) { goto err; }
						done=1;
					} else if (!*p) {
						/* unterminated quotes */
						goto err;
					} else {
						current = yaslcatlen(current, p, 1);
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
						current = yaslcatlen(current, p, 1);
						break;
					}
				}
				if (*p) { p++; }
			}
			/* add the token to the vector */

			char ** tmp = realloc(vector, (unsigned long)((*argc) + 1) * (sizeof (char *)));
			if (!tmp) {
				goto err;
			}
			vector = tmp;

			vector[*argc] = current;
			(*argc)++;
			current = NULL;
		} else {
			/* Even on empty input string return something not NULL. */
			if (!vector) { vector = malloc(sizeof(void*)); }
			return vector;
		}
	}

err:
	while((*argc)--) {
		yaslfree(vector[*argc]);
	}
	free(vector);
	if (current) { yaslfree(current); }
	*argc = 0;
	return NULL;
}

/* Split 's' with separator in 'sep'. An array
 * of yasl strings is returned. *count will be set
 * by reference to the number of tokens returned.
 *
 * On out of memory, zero length string, zero length
 * separator, NULL is returned.
 *
 * Note that 'sep' is able to split a string using
 * a multi-character separator. For example
 * yaslsplit("foo_-_bar", "_-_"); will return two
 * elements "foo" and "bar".
 *
 * This version of the function is binary-safe but
 * requires length arguments. yaslsplit() is just the
 * same function but for zero-terminated strings.
 */
yastr *
yaslsplitlen(const char * s, size_t len, const char * sep, size_t seplen, size_t * count) {
	size_t elements = 0, slots = 5, start = 0;
	yastr * tokens;

	if (seplen < 1) { return NULL; }

	tokens = malloc(sizeof(yastr)*slots);
	if (!tokens) { return NULL; }

	if (len == 0) {
		*count = 0;
		return tokens;
	}
	for (size_t j = 0; j < (len - (seplen - 1)); j++) {
		/* make sure there is room for the next element and the final one */
		if (slots < elements + 2) {
			yastr * newtokens;

			slots *= 2;
			newtokens = realloc(tokens, sizeof(yastr) * slots);
			if (!newtokens) { goto cleanup; }
			tokens = newtokens;
		}
		/* search the separator */
		if ((seplen == 1 && *(s + j) == sep[0]) || (memcmp(s + j, sep, seplen) == 0)) {
			tokens[elements] = yaslnew(s + start, (size_t)(j - start));
			if (!tokens[elements]) { goto cleanup; }
			elements++;
			start = j + seplen;
			j = j + seplen - 1; /* skip the separator */
		}
	}
	/* Add the final element. We are sure there is room in the tokens array. */
	tokens[elements] = yaslnew(s + start, (size_t)(len - start));
	if (!tokens[elements]) { goto cleanup; }
	elements++;
	*count = elements;
	return tokens;

cleanup:
	{
		for (size_t i = 0; i < elements; i++) {
			yaslfree(tokens[i]);
		}
		free(tokens);
		*count = 0;
		return NULL;
	}
}

// Concatenation //

/* Append the specified null termianted C string to the yasl string 's'.
 *
 * After the call, the passed yasl string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
yastr
yaslcat(yastr s, const char * t) {
	return yaslcatlen(s, t, strlen(t));
}

/* Append the specified yasl string 't' to the existing yasl string 's'.
 *
 * After the call, the modified yasl string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
yastr
yaslcatyasl(yastr s, const yastr t) {
	return yaslcatlen(s, t, yasllen(t));
}

/* Append the specified binary-safe string pointed by 't' of 'len' bytes to the
 * end of the specified yasl string 's'.
 *
 * After the call, the passed yasl string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
yastr
yaslcatlen(yastr s, const void * t, size_t len) {
	struct yastrhdr * sh;
	size_t curlen = yasllen(s);

	s = yaslMakeRoomFor(s, len);
	if (!s) { return NULL; }
	sh = yaslheader(s);
	memcpy(s + curlen, t, len);
	sh->len = curlen + len;
	sh->free = sh->free - len;
	s[curlen + len] = '\0';
	return s;
}

/* Append to the yasl string "s" an escaped string representation where
 * all the non-printable characters (tested with isprint()) are turned into
 * escapes in the form "\n\r\a...." or "\x<hex-number>".
 *
 * After the call, the modified yasl string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
yastr
yaslcatrepr(yastr s, const char * p, size_t len) {
	s = yaslcatlen(s, "\"", 1);
	while(len--) {
		switch(*p) {
		case '\\':
		case '"':
			s = yaslcatprintf(s, "\\%c", *p);
			break;
		case '\n': s = yaslcatlen(s, "\\n", 2); break;
		case '\r': s = yaslcatlen(s, "\\r", 2); break;
		case '\t': s = yaslcatlen(s, "\\t", 2); break;
		case '\a': s = yaslcatlen(s, "\\a", 2); break;
		case '\b': s = yaslcatlen(s, "\\b", 2); break;
		default:
			if (isprint(*p)) {
				s = yaslcatprintf(s, "%c", *p);
			} else {
				s = yaslcatprintf(s, "\\x%02x", (unsigned char)*p);
				break;
			}
		}
		p++;
	}
	return yaslcatlen(s, "\"", 1);
}

/* Like yaslcatpritf() but gets va_list instead of being variadic. */
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
yastr
yaslcatvprintf(yastr s, const char * fmt, va_list ap) {
	va_list cpy;
	char * buf, * t;
	size_t buflen = 16;

	while(1) {
		buf = malloc(buflen);
		if (!buf) { return NULL; }
		buf[buflen - 2] = '\0';
		va_copy(cpy, ap);
		vsnprintf(buf, buflen, fmt, cpy);
		va_end(cpy);
		if (buf[buflen - 2] != '\0') {
			free(buf);
			buflen *= 2;
			continue;
		}
		break;
	}
	t = yaslcat(s, buf);
	free(buf);
	return t;
}
#pragma GCC diagnostic warning "-Wformat-nonliteral"

/* Append to the yasl string 's' a string obtained using printf-alike format
 * specifier.
 *
 * After the call, the modified yasl string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = yaslempty("Sum is: ");
 * s = yaslcatprintf(s, "%d + %d = %d", a, b, a + b).
 *
 * Often you need to create a string from scratch with the printf-alike
 * format. When this is the need, just use yaslempty() as the target string:
 *
 * s = yaslcatprintf(yaslempty(), "... your format ...", args);
 */
yastr
yaslcatprintf(yastr s, const char * fmt, ...) {
	va_list ap;
	char * t;
	va_start(ap, fmt);
	t = yaslcatvprintf(s, fmt, ap);
	va_end(ap);
	return t;
}


// Freeing //

/* Free a yasl string. No operation is performed if 's' is NULL. */
void
yaslfree(yastr s) {
	if (!s) { return; }
	free(yaslheader(s));
}

/* Free the result returned by yaslsplitlen(), or do nothing if 'tokens' is NULL. */
void
yaslfreesplitres(yastr * tokens, size_t count) {
	if (!tokens) { return; }
	while(count--) {
		yaslfree(tokens[count]);
	}
	free(tokens);
}


// Low-level functions //

/* Return the total size of the allocation of the specifed yasl string,
 * including:
 * 1) The yasl header before the pointer.
 * 2) The string.
 * 3) The free buffer at the end if any.
 * 4) The implicit null term.
 */
size_t
yaslAllocSize(yastr s) {
	struct yastrhdr * sh = yaslheader(s);

	return sizeof(*sh) + sh->len + sh->free + 1;
}

/* Increment the yasl string length and decrements the left free space at the
 * end of the string according to 'incr'. Also set the null term in the new end
 * of the string.
 *
 * This function is used in order to fix the string length after the
 * user calls yaslMakeRoomFor(), writes something after the end of
 * the current string, and finally needs to set the new length.
 *
 * Usage example:
 *
 * Using yaslIncrLen() and yaslMakeRoomFor() it is possible to mount the
 * following schema, to cat bytes coming from the kernel to the end of a
 * yasl string without copying into an intermediate buffer:
 *
 * oldlen = yasllen(s);
 * s = yaslMakeRoomFor(s, BUFFER_SIZE);
 * nread = read(fd, s + oldlen, BUFFER_SIZE);
 * ... check for nread <= 0 and handle it ...
 * yaslIncrLen(s, nread);
 */
void
yaslIncrLen(yastr s, size_t incr) {
	struct yastrhdr * sh = yaslheader(s);

	assert(sh->free >= incr);
	sh->len += incr;
	sh->free -= incr;
	s[sh->len] = '\0';
}

/* Enlarge the free space at the end of the yasl string so that the caller
 * is sure that after calling this function can overwrite up to addlen
 * bytes after the end of the string, plus one more byte for nul term.
 *
 * Note: this does not change the *length* of the yasl string as returned
 * by yasllen(), but only the free buffer space we have. */
yastr
yaslMakeRoomFor(yastr s, size_t addlen) {
	struct yastrhdr * sh, * newsh;
	size_t free = yaslavail(s);
	size_t len, newlen;

	if (free >= addlen) { return s; }
	len = yasllen(s);
	sh = yaslheader(s);
	newlen = (len + addlen);
	if (newlen < YASL_MAX_PREALLOC) {
		newlen *= 2;
	} else {
		newlen += YASL_MAX_PREALLOC;
	}
	newsh = realloc(sh, sizeof(struct yastrhdr) + newlen + 1);
	if (!newsh) { return NULL; }

	newsh->free = newlen - len;
	return newsh->buf;
}

/* Reallocate the yasl string so that it has no free space at the end. The
 * contained string remains not altered, but next concatenation operations
 * will require a reallocation.
 *
 * After the call, the passed yasl string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
yastr
yaslRemoveFreeSpace(yastr s) {
	struct yastrhdr * sh = yaslheader(s);

	struct yastrhdr * tmp = realloc(sh, sizeof(struct yastrhdr) + sh->len + 1);
	if (tmp) {
		sh = tmp;
		sh->free = 0;
	}

	return sh->buf;
}


// Low-level helper functions //

/* Helper function for yaslsplitargs() that returns non zero if 'c'
 * is a valid hex digit. */
int is_hex_digit(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
	       (c >= 'A' && c <= 'F');
}

/* Helper function for yaslsplitargs() that converts a hex digit into an
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

