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
#include <stdio.h>
#include <string.h>

#include "twbctf.h"
#include "test.c"

// Forward Declarations //
#define TEST_COUNT ((sizeof test_list)/(sizeof (struct test)))

// Run Suite //
signed
main (void) {
	int print_width = 0;
	for (size_t i = 0; i < TEST_COUNT; i++) {
		int length = (int)strlen(test_list[i].desc);
		if (length > print_width) {
			print_width = length;
		}
	}

	for (size_t i = 0; i < TEST_COUNT; i++) {
		printf("Testing %-*s\t\t[ PEND ]\r", print_width, test_list[i].desc);
		char * result = (test_list[i].func() ? "\x1b[32mPASS" : "\x1b[31mFAIL");
		printf("Testing %-*s\t\t[ %s \x1b[0m]\n", print_width, test_list[i].desc, result);
	}

	return 0;
}
