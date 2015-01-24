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

/* Create a new yasl string, using `initlen` bytes from the `init` pointer to
 * initialize it with.
 */
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
	if (!s) { return NULL; }

	return yaslnew(s, yasllen(s));
}

/* Create an empty (zero length) yasl string. */
yastr
yaslempty(void) {
	return yaslnew("", 0);
}

/* Create a yasl string from a long long value. */
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

/* Modify a yasl string in-place to make it empty (zero length). */
void
yaslclear(yastr s) {
	if (!s) { return; }

	struct yastrhdr * sh = yaslheader(s);
	sh->free += sh->len;
	sh->len = 0;
	sh->buf[0] = '\0';
}

/* Grow the yasl string to have the specified length. Bytes that were not part
 * of the original length of the yasl string will be set to zero.
 */
yastr
yaslgrowzero(yastr s, size_t len) {
	if (!s) { return NULL; }

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
 * safe string pointed by 't' of length 'len' bytes.
 */
yastr
yaslcpylen(yastr s, const char * t, size_t len) {
	if (!s || !t) { return NULL; }

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
	if (!s || !t) { return NULL; }

	return yaslcpylen(s, t, strlen(t));
}

/* Join an array of C strings using the specified separator (also a C string).
 * Returns the result as a yasl string. */
yastr
yasljoin(char ** argv, int argc, char * sep, size_t seplen) {
	if (!argv || !sep) { return NULL; }

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
	if (!argv || !sep) { return NULL; }

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
 */
yastr
yaslmapchars(yastr s, const char * from, const char * to, size_t setlen) {
	if (!s || !from || !to) { return NULL; }

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
 */
void
yaslrange(yastr s, ptrdiff_t start, ptrdiff_t end) {
	if (!s) { return; }

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
	if (!s) { return; }

	for (size_t j = 0; j < yasllen(s); j++) {
		s[j] = (char)tolower(s[j]);
	}
}

/* Apply toupper() to every character of the yasl string 's'. */
void
yasltoupper(yastr s) {
	if (!s) { return; }

	for (size_t j = 0; j < yasllen(s); j++) {
		s[j] = (char)toupper(s[j]);
	}
}

/* Remove the part of the string from left and from right composed just of
 * contiguous characters found in 'cset', that is a null terminted C string.
 */
void
yasltrim(yastr s, const char * cset) {
	if (!s || !cset) { return; }

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

/* Set the yasl string length to the length as obtained with strlen(). */
void
yaslupdatelen(yastr s) {
	if (!s) { return; }

	struct yastrhdr * sh = yaslheader(s);
	size_t reallen = strlen(s);
	sh->free += (sh->len - reallen);
	sh->len = reallen;
}

/* Split a line into arguments, where every argument can be in the
 * following programming-language REPL-alike form:
 *
 * foo bar "newline are supported\n" and "\xff\x00otherstuff"
 */
yastr *
yaslsplitargs(const char * line, int * argc) {
	if (!line || !argc) { return NULL; }

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

/* Split 's' with separator in 'sep'. */
yastr *
yaslsplitlen(const char * s, size_t len, const char * sep, size_t seplen, size_t * count) {
	if (!s || !sep || !count) { return NULL; }

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

/* Append the specified null termianted C string to the yasl string 's'. */
yastr
yaslcat(yastr s, const char * t) {
	if (!s || !t) { return NULL; }

	return yaslcatlen(s, t, strlen(t));
}

/* Append the specified yasl string 't' to the existing yasl string 's'. */
yastr
yaslcatyasl(yastr s, const yastr t) {
	if (!s || !t) { return NULL; }

	return yaslcatlen(s, t, yasllen(t));
}

/* Append the specified binary-safe string pointed by 't' of 'len' bytes to the
 * end of the specified yasl string 's'.
 */
yastr
yaslcatlen(yastr s, const void * t, size_t len) {
	if (!s || !t) { return NULL; }

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
 */
yastr
yaslcatrepr(yastr s, const char * p, size_t len) {
	if (!s || !p) { return NULL; }

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
	if (!s || !fmt) { return NULL; }

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
 */
yastr
yaslcatprintf(yastr s, const char * fmt, ...) {
	if (!s || !fmt) { return NULL; }

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
	if (s) {
		free(yaslheader(s));
	}
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
	if (!s) { return 0; }

	struct yastrhdr * sh = yaslheader(s);

	return sizeof(*sh) + sh->len + sh->free + 1;
}

/* Increment the yasl string length and decrements the left free space at the
 * end of the string according to 'incr'. Also set the null term in the new end
 * of the string.
 */
void
yaslIncrLen(yastr s, size_t incr) {
	if (!s) { return; }

	struct yastrhdr * sh = yaslheader(s);

	assert(sh->free >= incr);
	sh->len += incr;
	sh->free -= incr;
	s[sh->len] = '\0';
}

/* Enlarge the free space at the end of the yasl string so that the caller
 * is sure that after calling this function can overwrite up to addlen
 * bytes after the end of the string, plus one more byte for nul term.
 */
yastr
yaslMakeRoomFor(yastr s, size_t addlen) {
	if (!s) { return NULL; }

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
 */
yastr
yaslRemoveFreeSpace(yastr s) {
	if (!s) { return NULL; }

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
 * is a valid hex digit.
 */
int is_hex_digit(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
	       (c >= 'A' && c <= 'F');
}

/* Helper function for yaslsplitargs() that converts a hex digit into an
 * integer from 0 to 15
 */
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

