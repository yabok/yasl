/*******************************************************************\
* A small, C test framework                                         *
* Copyright (C) 2013-2015, Sam Stuewe                               *
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
\*******************************************************************/

// Libraries //
#include "twbctf.h"  // testing forward declarations
#include "tests.c"   // tests

// Run Suite //
signed
main (void) {

    const size_t TC = (sizeof test_list)/(sizeof (struct test));
    signed results [TC], ret = 0;
    uint16_t maxl = 0;

    for ( size_t i = 0; i < TC; i ++ ) {
        uint16_t cur = (uint16_t )strlen(test_list[i].desc);
        maxl = cur > maxl ? cur : maxl;
    }

    size_t p = 0, f = 0;

    for ( size_t i = 0; i < TC; i ++ ) {
        results[i] = test_list[i].func();
        results[i] ? ++f : ++p;
        putchar(!results[i] ? '.' : '!');
        ret = ret || results[i];
        fflush(stdout);
    } printf("\n\n%zu Passed, %zu Failed", p, f);

    if ( ret ) {
        printf(":\n");
        for ( size_t i = 0; i < TC; i ++ ) {
            if ( results[i] ) {
                printf(" %s returned %d\n", test_list[i].desc, results[i]);
            }
        }
    } else {
        putchar('\n');
    } return ret;
}

// vim: set ts=4 sw=4 et:
