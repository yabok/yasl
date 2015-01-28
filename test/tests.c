#include <yasl.h>

#include "twbctf.h"

bool check_string_length             (void);
bool create_with_length              (void);
bool string_concat                   (void);
bool yaslcpy_against_longer_str      (void);
bool yaslcpy_against_shorter_str     (void);
bool yaslcatprintf_base_case         (void);
bool yasltrim_trims_correctly        (void);
bool yaslrange_one_one               (void);
bool yaslrange_one_none              (void);
bool yaslrange_ntwo_none             (void);
bool yaslrange_two_one               (void);
bool yaslrange_one_hund              (void);
bool yaslrange_hund_hund             (void);
bool yaslcmp_foo_foa                 (void);
bool yaslcmp_aar_bar                 (void);
bool yaslcmp_bar_bar                 (void);
bool yaslcmp_same_prefix_long_first  (void);
bool yaslcmp_same_prefix_long_second (void);
bool yaslcatrepr_test                (void);
bool yaslnew_check_free_len          (void);
bool yaslMakeRoomFor_test            (void);
bool yaslIncrLen_content             (void);
bool yaslIncrLen_len                 (void);
bool yaslIncrLen_free                (void);
bool test_yasltolower                (void);
bool test_yasltoupper                (void);

const struct test test_list [] = {
	{ "create a string and obtain the length",      check_string_length             },
	{ "create a string with specified length",      create_with_length              },
	{ "string concatenation",                       string_concat                   },
	{ "yaslcpy() against a longer string",          yaslcpy_against_longer_str      },
	{ "yaslcpy() against a shorter string",         yaslcpy_against_shorter_str     },
	{ "basic yaslcatprintf() usecase",              yaslcatprintf_base_case         },
	{ "yasltrim() trims correctly",                 yasltrim_trims_correctly        },
	{ "yaslrange(..., 1, 1)",                       yaslrange_one_one               },
	{ "yaslrange(..., 1, -1)",                      yaslrange_one_none              },
	{ "yaslrange(..., -2, -1)",                     yaslrange_ntwo_none             },
	{ "yaslrange(..., 2, 1)",                       yaslrange_two_one               },
	{ "yaslrange(..., 1, 100)",                     yaslrange_one_hund              },
	{ "yaslrange(..., 100, 100)",                   yaslrange_hund_hund             },
	{ "yaslcmp(foo, foa)",                          yaslcmp_foo_foa                 },
	{ "yaslcmp(aar, bar)",                          yaslcmp_aar_bar                 },
	{ "yaslcmp(bar, bar)",                          yaslcmp_bar_bar                 },
	{ "yaslcmp with identical prefix, long first",  yaslcmp_same_prefix_long_first  },
	{ "yaslcmp with identical prefix, long second", yaslcmp_same_prefix_long_second },
	{ "yaslcatrepr(...data...)",                    yaslcatrepr_test                },
	{ "yaslnew() free/len buffers",                 yaslnew_check_free_len          },
	{ "yaslMakeRoomFor()",                          yaslMakeRoomFor_test            },
	{ "content after yaslIncrLen()",                yaslIncrLen_content             },
	{ "len after yaslIncrLen()",                    yaslIncrLen_len                 },
	{ "free after yaslIncrLen()",                   yaslIncrLen_free                },
	{ "yasltolower()",                              test_yasltolower                },
	{ "yasltoupper()",                              test_yasltoupper                },
};

static inline void yaslfrees(yastr *string) { if (*string) yaslfree(*string); }
#define _yastr_cleanup_ __attribute__((cleanup(yaslfrees)))

bool
check_string_length(void) {
	_yastr_cleanup_ yastr x = yaslauto("foo");
	return (yasllen(x) == 3 && memcmp(x, "foo\0", 4) == 0);
}

bool
create_with_length(void) {
	_yastr_cleanup_ yastr x = yaslnew("foo", 2);
	return (yasllen(x) == 2 && memcmp(x, "fo\0", 3) == 0);
}

bool
string_concat(void) {
	_yastr_cleanup_ yastr x = yaslnew("foo", 2);
	x = yaslcat(x, "bar");
	return (yasllen(x) == 5 && memcmp(x, "fobar\0", 6) == 0);
}

bool
yaslcpy_against_longer_str(void) {
	_yastr_cleanup_ yastr x = yaslauto("foo");
	x = yaslcpy(x, "a");
	return (yasllen(x) == 1 && memcmp(x, "a\0", 2) == 0);
}

bool
yaslcpy_against_shorter_str(void) {
	_yastr_cleanup_ yastr x = yaslnew("foo",2);
	x = yaslcpy(x, "xxxxyyyyzzzz");
	return (yasllen(x) == 12 && memcmp(x, "xxxxyyyyzzzz\0", 12) == 0);
}

bool
yaslcatprintf_base_case(void) {
	_yastr_cleanup_ yastr x = yaslcatprintf(yaslempty(), "%d", 123);
	return (yasllen(x) == 3 && memcmp(x, "123\0", 4) == 0);
}

bool
yasltrim_trims_correctly(void) {
	_yastr_cleanup_ yastr x = yaslauto("xxciaoyy");
	yasltrim(x, "xy");
	return (yasllen(x) == 4 && memcmp(x, "ciao\0", 5) == 0);
}

bool
yaslrange_one_one(void) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, 1, 1);
	return (yasllen(y) == 1 && memcmp(y, "i\0", 2) == 0);
}

bool
yaslrange_one_none(void) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, 1, -1);
	return (yasllen(y) == 3 && memcmp(y, "iao\0", 4) == 0);
}

bool
yaslrange_ntwo_none(void) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, -2, -1);
	return (yasllen(y) == 2 && memcmp(y, "ao\0", 3) == 0);
}

bool
yaslrange_two_one(void) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, 2, 1);
	return (yasllen(y) == 0 && memcmp(y, "\0", 1) == 0);
}

bool
yaslrange_one_hund(void) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, 1, 100);
	return (yasllen(y) == 3 && memcmp(y, "iao\0", 4) == 0);
}

bool
yaslrange_hund_hund(void) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, 100, 100);
	return (yasllen(y) == 0 && memcmp(y, "\0", 1) == 0);
}

bool
yaslcmp_foo_foa(void) {
	_yastr_cleanup_ yastr x = yaslauto("foo");
	_yastr_cleanup_ yastr y = yaslauto("foa");
	return (yaslcmp(x, y) > 0);
}

bool
yaslcmp_aar_bar(void) {
	_yastr_cleanup_ yastr x = yaslauto("aar");
	_yastr_cleanup_ yastr y = yaslauto("bar");
	return (yaslcmp(x, y) < 0);
}

bool
yaslcmp_bar_bar(void) {
	_yastr_cleanup_ yastr x = yaslauto("bar");
	_yastr_cleanup_ yastr y = yaslauto("bar");
	return (yaslcmp(x, y) == 0);
}

bool
yaslcmp_same_prefix_long_first(void) {
	_yastr_cleanup_ yastr x = yaslauto("foobar");
	_yastr_cleanup_ yastr y = yaslauto("foo");
	return (yaslcmp(x, y) > 0);
}

bool
yaslcmp_same_prefix_long_second(void) {
	_yastr_cleanup_ yastr x = yaslauto("foo");
	_yastr_cleanup_ yastr y = yaslauto("foobar");
	return (yaslcmp(x, y) < 0);
}

bool
yaslcatrepr_test(void) {
	_yastr_cleanup_ yastr x = yaslnew("\a\n\0foo\r", 7);
	_yastr_cleanup_ yastr y = yaslcatrepr(yaslempty(), x, yasllen(x));
	return (memcmp(y, "\"\\a\\n\\x00foo\\r\"", 15) == 0);
}

bool
yaslnew_check_free_len(void) {
	_yastr_cleanup_ yastr x = yaslauto("0");
	struct yastrhdr *sh = (void*) (x-(sizeof(struct yastrhdr)));
	return (sh->len == 1 && sh->free == 0);
}

bool
yaslMakeRoomFor_test(void) {
	_yastr_cleanup_ yastr x = yaslauto("0");
	x = yaslMakeRoomFor(x, 1);
	struct yastrhdr *sh = (void*) (x-(sizeof(struct yastrhdr)));
	return (sh->len == 1 && sh->free > 0);
}

bool
yaslIncrLen_content(void) {
	_yastr_cleanup_ yastr x = yaslauto("0");
	x = yaslMakeRoomFor(x, 1);
	x[1] = '1';
	yaslIncrLen(x, 1);
	return (x[0] == '0' && x[1] == '1');
}

bool
yaslIncrLen_len(void) {
	_yastr_cleanup_ yastr x = yaslauto("0");
	x = yaslMakeRoomFor(x, 1);
	struct yastrhdr *sh = (void*) (x-(sizeof(struct yastrhdr)));
	x[1] = '1';
	yaslIncrLen(x, 1);
	return (sh->len == 2);
}

bool
yaslIncrLen_free(void) {
	_yastr_cleanup_ yastr x = yaslauto("0");
	x = yaslMakeRoomFor(x, 1);
	struct yastrhdr *sh = (void*) (x-(sizeof(struct yastrhdr)));
	size_t oldfree = sh->free;
	x[1] = '1';
	yaslIncrLen(x, 1);
	return (sh->free == oldfree-1);
}

bool
test_yasltolower(void) {
	_yastr_cleanup_ yastr x = yaslauto("0FoO1bar\n");
	yasltolower(x);
	return memcmp(x, "0foo1bar\n\0", 10) == 0;
}

bool
test_yasltoupper(void) {
	_yastr_cleanup_ yastr x = yaslauto("0FoO1bar\n");
	yasltoupper(x);
	return memcmp(x, "0FOO1BAR\n\0", 10) == 0;
}
