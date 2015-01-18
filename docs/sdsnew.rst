:orphan:

sdsnew
======

Synopsis
--------

.. c:function:: #include <sds.h>

.. c:function:: sds sdsnew(const char *init)
                sds sdsnewlen(const void *init, size_t initlen)


Description
-----------

The function :c:func:`sdsnew()` creates an SDS string from a C null terminated string.

The function :c:func:`sdsnewlen()` is similar to :c:func:`sdsnew()` but
instead of creating the string assuming that the input string is null
terminated, it gets an additional length parameter. This way you can create a
string from binary data.

Return value
------------

Upon successful completion, these functions will allocate and return a new SDS
string. If the allocation fails they will return :c:data:`NULL`.

See also
--------

:manpage:`sds(3)`
