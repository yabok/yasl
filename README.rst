===========================
 yasl |travis| |RDT| |cov|
===========================
.. |travis| image:: https://travis-ci.org/yabok/yasl.svg
   :alt: Travis Build Status
   :target: https://travis-ci.org/yabok/yasl
.. |RDT| image:: https://readthedocs.org/projects/yasl/badge/?version=latest
   :alt: Read-The-Docs Status
   :target: https://readthedocs.org/projects/yasl/?badge=latest
.. |cov| image:: https://scan.coverity.com/projects/3997/badge.svg
   :alt: Coverity Scan Status
   :target: https://scan.coverity.com/projects/3997
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

Documentation
=============

The new API documentation for yasl can either be found in ``docs/API.rst``, or
a built HTML version `here <http://yasl.readthedocs.org/en/latest/>`_. The API
documentation is very new and might not be as accurate as it should and also
does not yet have small code examples, though this is a planned addition.

For code examples the best sources right now is the tests in ``test/tests.c``
and in the old SDS README which can be found in ``docs/README.md.sds``.

License
=======

Every file in this repository, except for as otherwise specified in the file
itself or in this README if not possible, is licensed under a 2-clause BSD
license.
