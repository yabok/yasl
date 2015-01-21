======
 yasl
======
----------------------------------
 Yet Another String Library for C
----------------------------------

yasl is a simple dynamic string library for the C programming language,
targeting C99 and C11, and forked from the SDS library that was split out from
Redis.

Testing
=======

The yasl test suite is compiled with C99 and written using twbctf_.

To compile and run the test suite, run the following command::

    make test

.. _twbctf: https://github.com/HalosGhost/twbctf

Usage
=====

To use yasl copy the contents of the :literal:`src` directory into your sources
and include the :literal:`yasl.h` header into the files which need to use yasl.

The new API documentation isn’t finished yet, but a copy of the old SDS README
is available in :literal:`docs/README.md.sds`, and reading the yasl header and
test suite will provide a foundation for how to use it.

License
=======

Every file in this repository, except for as otherwise specified in the file
itself or in this README if not possible, is licensed under a 2-clause BSD
license.
