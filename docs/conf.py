#!/usr/bin/env python3
# -*- coding: utf-8 -*-

project = 'SDS'
master_doc = 'sds'
source_suffix = '.rst'

show_authors = False
today_fmt = '%d %b %Y'

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    ('sds', 'sds', 'Simple Dynamic String library for C', [], 3),
    ('sdsnew', 'sdsnew', 'Create a new SDS string', [], 3),
    ('sdsfree', 'sdsfree', 'Free an SDS string', [], 3),
]
