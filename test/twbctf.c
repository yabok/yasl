/*******************************************************************\
 * A small, C test framework                                         *
 * Copyright (C) 2013-2014, Sam Stuewe                               *
 *                                                                   *
 * This program is free software; you can redistribute it and/or     *
 * modify it under the terms of the GNU General Public License       *
 * as published by the Free Software Foundation; either version 2    *
 * of the License, or (at your option) any later version.            *
 *                                                                   *
 * This program is distributed in the hope that it will be useful,   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the      *
 * GNU General Public License for more details.                      *
 *                                                                   *
 * You should have received a copy of the GNU General Public License *
 * along with this program; if not, write to the Free Software       *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,                *
 * Boston, MA  02110-1301, USA.                                      *
\*********************************************************************/

// Libraries //
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "twbctf.h"
#include "test.c"

// Run Suite //
int32_t
main(signed argc, char * argv []) {

	const size_t tc = (sizeof test_list)/(sizeof (struct test));
	bool results [tc];
	bool shortened = (argc > 1 && *(int16_t * )argv[1] == *(int16_t * )"-s");
	int8_t maxl = 0;
	uint8_t ret = 0;

	for ( size_t i = 0; i < tc; i ++ ) {
		int8_t cur = (int8_t )strlen(test_list[i].desc);
		maxl = cur > maxl ? cur : maxl;
	}

	for ( size_t i = 0; i < tc; i ++ ) {
		if ( shortened ) {
			results[i] = test_list[i].func();
			putchar(results[i] ? '.' : '!');
			ret = results[i] ? ret : 1;
		} else {
			printf("Testing %-*s\t[ PEND ]\r", maxl, test_list[i].desc);
			bool result = test_list[i].func();
			char * r = result ? "\x1b[32mPASS" : "\x1b[31mFAIL";
			printf("Testing %-*s\t[ %s \x1b[0m]\n", maxl, test_list[i].desc, r);
			ret = result ? ret : 1;
		}
	}
	if ( shortened ) {
		printf("\x1b[0m\n\nFailed Tests:\n");
		for ( size_t i = 0; i < tc; i ++ ) {
			!results[i] ? printf(" %s\n", test_list[i].desc) : printf("");
		}
	}

	return ret;
}
