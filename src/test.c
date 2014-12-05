#include <stdbool.h>
#include <string.h>

#include "sds.h"
#include "twbctf.h"

bool check_string_length        (void);
bool create_with_length         (void);
bool string_concat              (void);
bool sdscpy_against_longer_str  (void);
bool sdscpy_against_shorter_str (void);
bool sdscatprintf_base_case     (void);
bool sdstrim_trims_correctly    (void);
bool sdsrange_one_one           (void);
bool sdsrange_one_none          (void);
bool sdsrange_ntwo_none         (void);
bool sdsrange_two_one           (void);
bool sdsrange_one_hund          (void);
bool sdsrange_hund_hund         (void);
bool sdscmp_foo_foa             (void);
bool sdscmp_aar_bar             (void);
bool sdscmp_bar_bar             (void);
bool sdscatrepr_test            (void);
bool sdsnew_check_free_len      (void);
bool sdsMakeRoomFor_test        (void);
bool sdsIncrLen_content         (void);
bool sdsIncrLen_len             (void);
bool sdsIncrLen_free            (void);

struct test test_list [] = {
	{ "create a string and obtain the length", check_string_length        },
	{ "create a string with specified length", create_with_length         },
	{ "string concatenation",                  string_concat              },
	{ "sdscpy() against a longer string",      sdscpy_against_longer_str  },
	{ "sdscpy() against a shorter string",     sdscpy_against_shorter_str },
	{ "basic sdscatprintf() usecase",          sdscatprintf_base_case     },
	{ "sdstrim() trims correctly",             sdstrim_trims_correctly    },
	{ "sdsrange(..., 1, 1)",                   sdsrange_one_one           },
	{ "sdsrange(..., 1, -1)",                  sdsrange_one_none          },
	{ "sdsrange(..., -2, -1)",                 sdsrange_ntwo_none         },
	{ "sdsrange(..., 2, 1)",                   sdsrange_two_one           },
	{ "sdsrange(..., 1, 100)",                 sdsrange_one_hund          },
	{ "sdsrange(..., 100, 100)",               sdsrange_hund_hund         },
	{ "sdscmp(foo, foa)",                      sdscmp_foo_foa             },
	{ "sdscmp(aar, bar)",                      sdscmp_aar_bar             },
	{ "sdscmp(bar, bar)",                      sdscmp_bar_bar             },
	{ "sdscatrepr(...data...)",                sdscatrepr_test            },
	{ "sdsnew() free/len buffers",             sdsnew_check_free_len      },
	{ "sdsMakeRoomFor()",                      sdsMakeRoomFor_test        },
	{ "content after sdsIncrLen()",            sdsIncrLen_content         },
	{ "len after sdsIncrLen()",                sdsIncrLen_len             },
	{ "free after sdsIncrLen()",               sdsIncrLen_free            },
};

static inline void sdsfrees(sds *string) { if (*string) sdsfree(*string); }
#define _sds_cleanup_ __attribute__((cleanup(sdsfrees)))

bool
check_string_length(void) {
	_sds_cleanup_ sds x = sdsnew("foo");

	return (sdslen(x) == 3 && memcmp(x, "foo\0", 4) == 0);
}

bool
create_with_length(void) {
	_sds_cleanup_ sds x = sdsnewlen("foo", 2);

	return (sdslen(x) == 2 && memcmp(x, "fo\0", 3) == 0);
}

bool
string_concat(void) {
	_sds_cleanup_ sds x = sdsnewlen("foo", 2);
	x = sdscat(x, "bar");

	return (sdslen(x) == 5 && memcmp(x, "fobar\0", 6) == 0);
}

bool
sdscpy_against_longer_str(void) {
	_sds_cleanup_ sds x = sdsnew("foo");
	x = sdscpy(x, "a");

	return (sdslen(x) == 1 && memcmp(x, "a\0", 2) == 0);
}

bool
sdscpy_against_shorter_str(void) {
	_sds_cleanup_ sds x = sdsnewlen("foo",2);
	x = sdscpy(x, "xxxxyyyyzzzz");

	return (sdslen(x) == 12 && memcmp(x, "xxxxyyyyzzzz\0", 12) == 0);
}

bool
sdscatprintf_base_case(void) {
	_sds_cleanup_ sds x = sdscatprintf(sdsempty(), "%d", 123);

	return (sdslen(x) == 3 && memcmp(x, "123\0", 4) == 0);
}

bool
sdstrim_trims_correctly(void) {
	_sds_cleanup_ sds x = sdsnew("xxciaoyy");
	sdstrim(x, "xy");

	return (sdslen(x) == 4 && memcmp(x, "ciao\0", 5) == 0);
}

bool
sdsrange_one_one (void) {
	_sds_cleanup_ sds x = sdsnew("ciao");
	_sds_cleanup_ sds y = sdsdup(x);
	sdsrange(y, 1, 1);

	return (sdslen(y) == 1 && memcmp(y, "i\0", 2) == 0);
}

bool
sdsrange_one_none (void) {
	_sds_cleanup_ sds x = sdsnew("ciao");
	_sds_cleanup_ sds y = sdsdup(x);
	sdsrange(y, 1, -1);

	return (sdslen(y) == 3 && memcmp(y, "iao\0", 4) == 0);
}

bool
sdsrange_ntwo_none (void) {
	_sds_cleanup_ sds x = sdsnew("ciao");
	_sds_cleanup_ sds y = sdsdup(x);
	sdsrange(y, -2, -1);

	return (sdslen(y) == 2 && memcmp(y, "ao\0", 3) == 0);
}

bool
sdsrange_two_one (void) {
	_sds_cleanup_ sds x = sdsnew("ciao");
	_sds_cleanup_ sds y = sdsdup(x);
	sdsrange(y, 2, 1);

	return (sdslen(y) == 0 && memcmp(y, "\0", 1) == 0);
}

bool
sdsrange_one_hund (void) {
	_sds_cleanup_ sds x = sdsnew("ciao");
	_sds_cleanup_ sds y = sdsdup(x);
	sdsrange(y, 1, 100);

	return (sdslen(y) == 3 && memcmp(y, "iao\0", 4) == 0);
}

bool
sdsrange_hund_hund (void) {
	_sds_cleanup_ sds x = sdsnew("ciao");
	_sds_cleanup_ sds y = sdsdup(x);
	sdsrange(y, 100, 100);

	return (sdslen(y) == 0 && memcmp(y, "\0", 1) == 0);
}

bool
sdscmp_foo_foa (void) {
	_sds_cleanup_ sds x = sdsnew("foo");
	_sds_cleanup_ sds y = sdsnew("foa");

	return (sdscmp(x, y) > 0);
}

bool
sdscmp_aar_bar (void) {
	_sds_cleanup_ sds x = sdsnew("aar");
	_sds_cleanup_ sds y = sdsnew("bar");

	return (sdscmp(x, y) < 0);
}

bool
sdscmp_bar_bar (void) {
	_sds_cleanup_ sds x = sdsnew("bar");
	_sds_cleanup_ sds y = sdsnew("bar");

	return (sdscmp(x, y) == 0);
}

bool
sdscatrepr_test (void) {
	_sds_cleanup_ sds x = sdsnewlen("\a\n\0foo\r", 7);
	_sds_cleanup_ sds y = sdscatrepr(sdsempty(), x, sdslen(x));
	return (memcmp(y, "\"\\a\\n\\x00foo\\r\"", 15) == 0);
}

bool
sdsnew_check_free_len (void) {
	_sds_cleanup_ sds x = sdsnew("0");
	struct sdshdr *sh = (void*) (x-(sizeof(struct sdshdr)));

	return (sh->len == 1 && sh->free == 0);
}

bool
sdsMakeRoomFor_test (void) {
	_sds_cleanup_ sds x = sdsnew("0");
	x = sdsMakeRoomFor(x, 1);
	struct sdshdr *sh = (void*) (x-(sizeof(struct sdshdr)));

	return (sh->len == 1 && sh->free > 0);
}

bool
sdsIncrLen_content (void) {
	_sds_cleanup_ sds x = sdsnew("0");
	x = sdsMakeRoomFor(x, 1);
	x[1] = '1';
	sdsIncrLen(x, 1);

	return (x[0] == '0' && x[1] == '1');
}

bool
sdsIncrLen_len (void) {
	_sds_cleanup_ sds x = sdsnew("0");
	x = sdsMakeRoomFor(x, 1);
	struct sdshdr *sh = (void*) (x-(sizeof(struct sdshdr)));
	x[1] = '1';
	sdsIncrLen(x, 1);

	return (sh->len == 2);
}

bool
sdsIncrLen_free (void) {
	_sds_cleanup_ sds x = sdsnew("0");
	x = sdsMakeRoomFor(x, 1);
	struct sdshdr *sh = (void*) (x-(sizeof(struct sdshdr)));
	int oldfree = sh->free;
	x[1] = '1';
	sdsIncrLen(x, 1);

	return (sh->free == oldfree-1);
}
