:orphan:

yaslnew
=======

Synopsis
--------

.. c:function:: #include <yasl.h>

.. c:function:: yastr yaslauto(const char *init)
                yastr yaslnew(const void *init, size_t initlen)


Description
-----------

The function :c:func:`yaslauto()` creates an yasl string from a C null
terminated string.

The function :c:func:`yaslnew()` is similar to :c:func:`yaslauto()` but instead
of creating the string assuming that the input string is null terminated, it
gets an additional length parameter. This way you can create a string from
binary data.

Return value
------------

Upon successful completion, these functions will allocate and return a new yasl
string. If the allocation fails they will return :c:data:`NULL`.

See also
--------

:manpage:`yasl(3)`
