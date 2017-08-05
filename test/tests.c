#include <stdbool.h>
#include <yasl.h>
#include "twbctf.h"

static inline void yaslfrees(yastr *string) { if (*string) yaslfree(*string); }
#define _yastr_cleanup_ __attribute__((cleanup(yaslfrees)))

declare_test(check_string_length) {
	_yastr_cleanup_ yastr x = yaslauto("foo");
	return yasllen(x) == 3 && memcmp(x, "foo\0", 4);
}

declare_test(create_with_length) {
	_yastr_cleanup_ yastr x = yaslnew("foo", 2);
	return yasllen(x) == 2 && memcmp(x, "fo\0", 3);
}

declare_test(string_concat) {
	_yastr_cleanup_ yastr x = yaslnew("foo", 2);
	x = yaslcat(x, "bar");
	return yasllen(x) == 5 && memcmp(x, "fobar\0", 6);
}

declare_test(yaslcpy_against_longer_str) {
	_yastr_cleanup_ yastr x = yaslauto("foo");
	x = yaslcpy(x, "a");
	return yasllen(x) == 1 && memcmp(x, "a\0", 2);
}

declare_test(yaslcpy_against_shorter_str) {
	_yastr_cleanup_ yastr x = yaslnew("foo",2);
	x = yaslcpy(x, "xxxxyyyyzzzz");
	return yasllen(x) == 12 && memcmp(x, "xxxxyyyyzzzz\0", 12);
}

declare_test(yaslcatprintf_base_case) {
	_yastr_cleanup_ yastr x = yaslcatprintf(yaslempty(), "%d", 123);
	return yasllen(x) == 3 && memcmp(x, "123\0", 4);
}

declare_test(yasltrim_trims_correctly) {
	_yastr_cleanup_ yastr x = yaslauto("xxciaoyy");
	yasltrim(x, "xy");
	return yasllen(x) == 4 && memcmp(x, "ciao\0", 5);
}

declare_test(yaslstrip_single) {
	_yastr_cleanup_ yastr x = yaslauto("xxyyxx");
	yaslstrip(x, "x");
	return yasllen(x) == 2 && memcmp(x, "yy\0", 3);
}

declare_test(yaslstrip_multiple) {
	_yastr_cleanup_ yastr x = yaslauto("xxyyz");
	yaslstrip(x, "xy");
	return yasllen(x) == 1 && memcmp(x, "z\0", 2);
}

declare_test(yaslstrip_all) {
	_yastr_cleanup_ yastr x = yaslauto("xxxxxxxx");
	yaslstrip(x, "x");
	return yasllen(x) == 0 && memcmp(x, "\0", 1);
}

declare_test(yaslstrip_nothing) {
	_yastr_cleanup_ yastr x = yaslauto("foo");
	yaslstrip(x, "x");
	return yasllen(x) == 3 && memcmp(x, "foo\0", 4);
}

declare_test(yaslrange_one_one) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, 1, 1);
	return yasllen(y) == 1 && memcmp(y, "i\0", 2);
}

declare_test(yaslrange_one_none) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, 1, -1);
	return yasllen(y) == 3 && memcmp(y, "iao\0", 4);
}

declare_test(yaslrange_ntwo_none) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, -2, -1);
	return yasllen(y) == 2 && memcmp(y, "ao\0", 3);
}

declare_test(yaslrange_two_one) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, 2, 1);
	return yasllen(y) == 0 && memcmp(y, "\0", 1);
}

declare_test(yaslrange_one_hund) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, 1, 100);
	return yasllen(y) == 3 && memcmp(y, "iao\0", 4);
}

declare_test(yaslrange_hund_hund) {
	_yastr_cleanup_ yastr x = yaslauto("ciao");
	_yastr_cleanup_ yastr y = yasldup(x);
	yaslrange(y, 100, 100);
	return yasllen(y) == 0 && memcmp(y, "\0", 1);
}

declare_test(yaslcmp_foo_foa) {
	_yastr_cleanup_ yastr x = yaslauto("foo");
	_yastr_cleanup_ yastr y = yaslauto("foa");
	return (!(yaslcmp(x, y) > 0));
}

declare_test(yaslcmp_aar_bar) {
	_yastr_cleanup_ yastr x = yaslauto("aar");
	_yastr_cleanup_ yastr y = yaslauto("bar");
	return (!(yaslcmp(x, y) < 0));
}

declare_test(yaslcmp_bar_bar) {
	_yastr_cleanup_ yastr x = yaslauto("bar");
	_yastr_cleanup_ yastr y = yaslauto("bar");
	return (!(yaslcmp(x, y) == 0));
}

declare_test(yaslcmp_same_prefix_long_first) {
	_yastr_cleanup_ yastr x = yaslauto("foobar");
	_yastr_cleanup_ yastr y = yaslauto("foo");
	return (!(yaslcmp(x, y) > 0));
}

declare_test(yaslcmp_same_prefix_long_second) {
	_yastr_cleanup_ yastr x = yaslauto("foo");
	_yastr_cleanup_ yastr y = yaslauto("foobar");
	return (!(yaslcmp(x, y) < 0));
}

declare_test(yaslcatrepr_test) {
	_yastr_cleanup_ yastr x = yaslnew("\a\n\0foo\r", 7);
	_yastr_cleanup_ yastr y = yaslcatrepr(yaslempty(), x, yasllen(x));
	return memcmp(y, "\"\\a\\n\\x00foo\\r\"", 15);
}

declare_test(yaslnew_check_free_len) {
	_yastr_cleanup_ yastr x = yaslauto("0");
	struct yastrhdr *sh = (void*) (x-(sizeof(struct yastrhdr)));
	return (!(sh->len == 1 && sh->free == 0));
}

declare_test(yaslMakeRoomFor_test) {
	_yastr_cleanup_ yastr x = yaslauto("0");
	x = yaslMakeRoomFor(x, 1);
	struct yastrhdr *sh = (void*) (x-(sizeof(struct yastrhdr)));
	return (!(sh->len == 1 && sh->free > 0));
}

declare_test(yaslIncrLen_content) {
	_yastr_cleanup_ yastr x = yaslauto("0");
	x = yaslMakeRoomFor(x, 1);
	x[1] = '1';
	yaslIncrLen(x, 1);
	return (!(x[0] == '0' && x[1] == '1'));
}

declare_test(yaslIncrLen_len) {
	_yastr_cleanup_ yastr x = yaslauto("0");
	x = yaslMakeRoomFor(x, 1);
	struct yastrhdr *sh = (void*) (x-(sizeof(struct yastrhdr)));
	x[1] = '1';
	yaslIncrLen(x, 1);
	return (!(sh->len == 2));
}

declare_test(yaslIncrLen_free) {
	_yastr_cleanup_ yastr x = yaslauto("0");
	x = yaslMakeRoomFor(x, 1);
	struct yastrhdr *sh = (void*) (x-(sizeof(struct yastrhdr)));
	size_t oldfree = sh->free;
	x[1] = '1';
	yaslIncrLen(x, 1);
	return (!(sh->free == oldfree-1));
}

declare_test(yasltolower_ascii_digits) {
	_yastr_cleanup_ yastr x = yaslauto("0FoO1bar\n");
	yasltolower(x);
	return (!memcmp(x, "0foo1bar\n\0", 10) == 0);
}

declare_test(yasltoupper_ascii_digits) {
	_yastr_cleanup_ yastr x = yaslauto("0FoO1bar\n");
	yasltoupper(x);
	return (!memcmp(x, "0FOO1BAR\n\0", 10) == 0);
}

const struct test test_list [] = {
	{ "create a string and obtain the length",      check_string_length             },
	{ "create a string with specified length",      create_with_length              },
	{ "string concatenation",                       string_concat                   },
	{ "yaslcpy() against a longer string",          yaslcpy_against_longer_str      },
	{ "yaslcpy() against a shorter string",         yaslcpy_against_shorter_str     },
	{ "basic yaslcatprintf() usecase",              yaslcatprintf_base_case         },
	{ "yasltrim() trims correctly",                 yasltrim_trims_correctly        },
	{ "yaslstrip() strips correctly",               yaslstrip_single                },
	{ "yaslstrip() strips multiple chars",          yaslstrip_multiple              },
	{ "yaslstrip() with everything to strip",       yaslstrip_all                   },
	{ "yaslstrip() with nothing to strip",          yaslstrip_nothing               },
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
	{ "yasltolower() with ASCII and digits",        yasltolower_ascii_digits        },
	{ "yasltoupper() with ASCII and digits",        yasltoupper_ascii_digits        },
};
