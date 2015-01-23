#!/usr/bin/env python3
# -*- coding: utf-8 -*-

project = 'YASL'
copyright = '2014-2015, The yasl developers'
master_doc = 'index'
source_suffix = '.rst'

show_authors = False
today_fmt = '%d %b %Y'

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
exclude_patterns = ['_build']

highlight_language = 'c'

# Output file base name for HTML help builder.
htmlhelp_basename = 'yasldoc'

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    ('yasl',     'yasl',     'Yet Another String Library for C', [], 3),
    ('yaslnew',  'yaslnew',  'Create a new yasl string',         [], 3),
    ('yaslfree', 'yaslfree', 'Free a yasl string',               [], 3),
]
