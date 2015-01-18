:orphan:

sdsnew
======

Synopsis
--------

.. c:function:: #include <sds.h>

.. c:function:: void sdsfree(sds s)


Description
-----------

The function :c:func:`sdsfree()` frees an SDS string, doing nothing if passed a NULL pointer.

See also
--------

:manpage:`sds(3)`
