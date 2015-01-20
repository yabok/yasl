===================
 API Documentation
===================

.. role:: c(code)
   :language: c

Data structures
===============

There are two data structures used in yasl. The first which is what all code
using yasl strings use is :c:`yastr` which is a :c:`char *` :c:`typedef`. Since
it's just a typedef its use is not strictly necessary, but is recommended to
make it clear that the given string is a yasl string.

The second data structure is the following struct:

.. code:: c

    struct yastrhdr {
        size_t len;
        size_t free;
        char buf[];
    };

The :c:`yastrhdr` struct is the header that exists before all yasl strings, and
which keeps track of the length and amount of free space available in the
string. All instances of :c:`yastr` are really pointers to to the :c:`char`
buffer in a :c:`yastrhdr` struct.

Due to the :c:`yastr` being a pointer to a member of a :c:`yastr` struct all
internal functions in yasl can use pointer arithmetic to get a pointer to the
:c:`yastrhdr` of a given :c:`yastr` string, which is why all functions in yasl
only requires a :c:`yastr` instead of a :c:`yastrhdr`

Initialization
==============

The initialization group contains the functions used to create new :c:`yastr`
strings.

yaslnew
~~~~~~~

.. code:: c

   yastr yaslnew(const void * init, size_t initlen)

The :c:`yaslnew()` function allocates a new :c:`yastr` of size :c:`initlen` and
using the contents of the :c:`init` argument to initialize the string with.


yaslauto
~~~~~~~~

.. code:: c

    yastr yaslauto(const char * s)

The :c:`yaslauto()` function uses strlen to get the length of the :c:`char *`
given and then uses :c:`yaslnew()` to create a new :c:`yastr`. If the :c:`char *`
given is an empty string it will fall back to creating a :c:`yastr` of size
zero.


yasldup
~~~~~~~

.. code:: c

    yastr yasldup(const yastr s)

The :c:`yasldup()` functions takes a :c:`yastr` and creates and returns a new
:c:`yastr` using the given string as an initializing value.

yaslempty
~~~~~~~~~

.. code:: c

    yastr yaslempty(void)

The :c:`yaslempty()` function creates and returns an empty :c:`yastr`


yaslfromlonglong
~~~~~~~~~~~~~~~~

.. code:: c

    yastr yaslfromlonglong(long long value)

The :c:`yaslfromlonglong()` function creates and returns a :c:`yastr` from a
long long value. This could be done with :c:`yaslcatprintf()` but the
implementation used in :c:`yaslfromlonglong()` is more specialized and thus
faster.

Querying
========

yaslcmp
~~~~~~~

.. code:: c

    int yaslcmp(const yastr s1, const yastr s2)

The :c:`yaslcmp()` function takes two :c:`yastr`'s and compares them using
:c:`memcmp()`. Its return values is lesser than, equal to, or greater than zero
if the first string is lesser than, equal to, or greater than the second
string.

If two strings share an identical prefix, but one of them has additional
characters at the end, the longer string is considered to be greater than the
shorter one.

yaslavail
~~~~~~~~~

.. code:: c

    size_t yaslavail(const yastr s)

The :c:`yaslavail()` function takes a :c:`yastr` and returns the amount of
space left available in the string's buffer before it will need to be
:c:`realloc()`'ed. This operation is fast since it just needs to return the
value of the :c:`avail` member of the :c:`yastrhdr` struct, which is updated
every time the :c:`yastr` is modified.

yasllen
~~~~~~~

.. code:: c

    size_t yasllen(const yastr s)

The :c:`yasllen()` function takes a :c:`yastr` and returns the length of the
string. This operation is fast and safe since it just needs to return the value
of the :c:`len` member of the :c:`yastrhdr` which is updated every time the
:c:`yastr` is modified, and thus the length is always known.

Modification
============

This group contains all the functions used for modification of :c:`yastr`
strings, with the exception of the concatenation functions in the next group.

yaslclear
~~~~~~~~~

.. code:: c

    void yaslclear(yastr s)

The :c:`yaslclear()` function takes a :c:`yastr` and clears it, setting the
length to zero and the first :c:`char` to :c:`NULL`. This function does not
either :c:`realloc()` or set all of the string to :c:`NULL` bytes, so it's
fast, but it will also not suffice if you need the string to be reset to all
:c:`NULL` bytes.

yaslgrowzero
~~~~~~~~~~~~

.. code:: c

    yastr yaslgrowzero(yastr s, size_t len)

The :c:`yaslgrowzero()` function takes a :c:`yastr` and a length arguments. If
the length argument is bigger than the current length of the string it will
grow the string to the given length and set all of the new length to zero, but
will not touch the content of original length of the string.

If the given length is smaller than the current length no operation is performed.

This function may :c:`realloc()` the string so all references to the original
:c:`yastr` should be treated as invalid and should be replaced with the one
returned by this function.

yaslcpylen
~~~~~~~~~~

.. code:: c

    yastr yaslcpylen(yastr s, const char * t, size_t len)

The :c:`yaslcpylen()` function copies :c:`len` bytes from the given :c:`char *`
to the given :c:`yastr`.

This function may :c:`realloc()` the string so all references to the original
:c:`yastr` should be treated as invalid and should be replaced with the one
returned by this function.

This function may also return :c:`NULL` in case the :c:`realloc()` call failed,
in which case the original :c:`yastr` references are still valid and should be
used.

yaslcpy
~~~~~~~

.. code:: c

    yastr yaslcpy(yastr s, const char * t)

The :c:`yaslcpy()` function copies the contents of the given :c:`char *` to a
:c:`yastr`. It is identical to the :c:`yaslcpylen()` function but instead of
explicitly giving a length parameter it will run the :c:`strlen()` function on
the :c:`char *` before calling the :c:`yaslcpylen()` function.

This function may :c:`realloc()` the string so all references to the original
:c:`yastr` should be treated as invalid and should be replaced with the one
returned by the function.

This function may also return :c:`NULL` in case the :c:`realloc()` call failed,
in which case the original :c:`yastr` references are still valid and should be
used.


yasljoin
~~~~~~~~

.. code:: c

    yastr yasljoin(char ** argv, int argc, char * sep, size_t seplen)

The :c:`yasljoin()` function joins an array of C strings using the specified
 C string separator, and returns the resulting string as a :c:`yastr`.

yasljoinyasl
~~~~~~~~~~~~

.. code:: c

    yastr yasljoinyasl(yastr * argv, int argc, const char * sep, size_t seplen)

The :c:`yasljoinyasl()` function join an array of :c:`yastr` using the
specified C string separator, and returns the resulting string as a new
:c:`yastr`.

yaslmapchars
~~~~~~~~~~~~

.. code:: c

    yastr yaslmapchars(yastr s, const char * from, const char * to, size_t setlen)

The :c:`yaslmapchars()` function replaces every occurrence of the set of
characters in the :c:`from` C string to the corresponding character in the
:c:`to` C string.

Since this function just maps one set of characters to another set of
characters it will never change the length of the string, so the existing
references to the string will continue being valid.

yaslrange
~~~~~~~~~

.. code:: c

    void yaslrange(yastr s, ptrdiff_t start, ptrdiff_t end)

The :c:`yaslrange()` function will destructively modify the :c:`yastr` to only
contain the substring marked by the given start and end arguments. The start
and end arguments may be negative, where ``-1`` means the last character, et
cetera. The given argument ranges are inclusive, so the start and end
characters will be included in the resulting string.

yasltolower
~~~~~~~~~~~

.. code:: c

    void yasltolower(yastr s)

The :c:`yasltolower()` function takes a :c:`yastr` and runs the :c:`tolower()`
function on each char of the string.

yasltoupper
~~~~~~~~~~~

.. code:: c

    void yasltoupper(yastr s)

The :c:`yasltoupper()` function takes a :c:`yastr` and runs the :c:`touppeupper()`
function on each char of the string.


yasltrim
~~~~~~~~

.. code:: c

    void yasltrim(yastr s, const char * cset)

The :c:`yasltrim()` function will trim the characters composed of just the
characters found in the :c:`cset` C string from the beginning and end of the
given :c:`yastr`, and returns a new :c:`yastr` without them.

This function destructively modifies the string and all references to the old
string will be invalid and should be updated to the returned one.

yaslupdatelen
~~~~~~~~~~~~~

.. code:: c

    :oid yaslupdatelen(yastr s)

The :c:`yaslupdatelen()` updates the :c:`len` of the string to the value
returned by :c:`strlen()`. This function is useful when the :c:`yastr` has been
modified by a function not aware of yasl strings, but since :c:`strlen()` works
on C strings it will not work properly on strings containing :c:`NULL`
characters.

yaslsplitargs
~~~~~~~~~~~~~

.. code:: c

    yastr * yaslsplitargs(const char * line, int * argc)

The :c:`yaslsplitargs()` function splits a C string into an array of :c:`yastr`
strings in the same way a shell would. The :c:`argc` pointer is set to the
number of members in the :c:`yastr` array.

The caller should free the resulting array of :c:`yastr` strings using the
:c:`yaslfreesplitres()` function.

The opposite of this function is provided by the :c:`yaslcatrepr()` function.

This function will return :c:`NULL` if the input contains unbalanced quoted or
closed quotes followed by a non-space character.

yaslsplitlen
~~~~~~~~~~~~

.. code:: c

    yastr * yaslsplitlen(const char * s, size_t len, const char * sep, size_t seplen, size_t * count)

The :c:`yaslsplitlen()` function splits the given C string using the :c:`sep` C
string as the separator. The :c:`count` pointer is set to the number of members
in the array of :c:`yastr` strings that is returned.

This function is binary safe, which is why it requires the length of the string
and separators, so both can contain binary data.

This function may return NULL on out of memory, or if a zero-length string or
separator was given.

Concatenation
=============

This group contains all of the functions used to concatenate two strings
together.

The functions in this section technically belongs to the modification group,
but it was split out into its own group due to the large amount of functions.

yaslcat
~~~~~~~

.. code:: c

    yastr yaslcat(yastr s, const char * t)

The :c:`yaslcat()` function appends the given C string to the :c:`yastr s`

This function may :c:`realloc()` the string so all references to the original
:c:`yastr` should be treated as invalid and should be replaced with the one
returned by the function.

yaslcatyasl
~~~~~~~~~~~

.. code:: c

    yastr yaslcatyasl(yastr s, const yastr t)

The :c:`yaslcat()` function appends the given :c:`yastr t` to the existing
:c:`yastr s`.

This function may :c:`realloc()` the string so all references to the original
:c:`yastr` should be treated as invalid and should be replaced with the one
returned by the function.

yaslcatlen
~~~~~~~~~~

.. code:: c

    yastr yaslcatlen(yastr s, const void * t, size_t len)

The :c:`yaslcatlen()` function appends the string :c:`t` of length :c:`len` to
the end of the specified :c:`yastr`.

This function is binary safe, which is why it requires the length of the string
as an argument.

This function may :c:`realloc()` the string so all references to the original
:c:`yastr` should be treated as invalid and should be replaced with the one
returned by this function.

yaslcatrepr
~~~~~~~~~~~

.. code:: c

    yastr yaslcatrepr(yastr s, const char * p, size_t len)

The :c:`yaslcatrepr()` function takes a C string and appends an escaped string
representation of it to the given :c:`yastr`. All non-printable characters are
turned into appropriate escape codes if existent, or a ``\x<hex>`` otherwise.

This function may :c:`realloc()` the string so all references to the original
:c:`yastr` should be treated as invalid and should be replaced with the one
returned by this function.

yaslcatvprintf
~~~~~~~~~~~~~~

.. code:: c

    yastr yaslcatvprintf(yastr s, const char * fmt, va_list ap)

The :c:`yaslcatprintf()` function appends a string obtained using a printf-like
format specifier to the given :c:`yastr`, taking an :c:`va_list` argument
instead of being a variadic function.

Often you need to create a new string with the printf-like format, and when
this is needed you can just use :c:`yaslempty()` as the target to create a new
empty one.

This function may :c:`realloc()` the string so all references to the original
:c:`yastr` should be treated as invalid and should be replaced with the one
returned by this function.


yaslcatprintf
~~~~~~~~~~~~~

.. code:: c

    yastr yaslcatprintf(yastr s, const char * fmt, ...)

The :c:`yaslcatprintf()` function appends a string obtained using a printf-like
format specifier to the given :c:`yastr`.

Often you need to create a new string with the printf-like format, and when
this is needed you can just use :c:`yaslempty()` as the target to create a new
empty one.

This function may :c:`realloc()` the string so all references to the original
:c:`yastr` should be treated as invalid and should be replaced with the one
returned by this function.

Freeing
=======

This group contains the functions used to free :c:`yastr` strings.

yaslfree
~~~~~~~~

.. code:: c

    void yaslfree(yastr s)

The :c:`yaslfree()` function frees a yasl string.

If the :c:`s` argument is NULL no operation is performed.

yaslfreesplitres
~~~~~~~~~~~~~~~~

.. code:: c

    void yaslfreesplitres(yastr * tokens, size_t count)

The :c:`yaslfreesplitres()` function frees the result of :c:`yaslsplitlen()`.

If the given :c:`yastr *` is :c:`NULL` no operation is performed.

Low-level functions
===================

This group contains the functions in the low-level API and should generally not
be used in client code.

yaslAllocSize
~~~~~~~~~~~~~

.. code:: c

    size_t yaslAllocSize(yastr s)

The :c:`yaslAllocSize()` function returns the total allocated size of the
specified yasl string, including the :c:`yastrhdr` and the full string buffer.

yaslIncrLen
~~~~~~~~~~~

.. code:: c

    void yaslIncrLen(yastr s, size_t incr)

The :c:`yaslIncrLen()` function increments the length and decrements the free
space members in the :c:`yastrhdr` of the given :c:`yastr` by the amount given
in :c:`incr`, and also sets the new end of the string to :c:`NULL`.

This function is used to fix the string length after calling
:c:`yaslMakeRoomFor` and then writing something to the end of the string.

yaslMakeRoomFor
~~~~~~~~~~~~~~~

.. code:: c

    yastr yaslMakeRoomFor(yastr s, size_t addlen)

The :c:`yaslMakeRoomFor()` function grows the free space at the end of the
given :c:`yastr` string so that the caller is sure that there is at least
:c:`addlen` bytes of space available at the end of the string.

This function does not update the len member of the string returned by
:c:`yasllen()`

yaslRemoveFreeSpace
~~~~~~~~~~~~~~~~~~~

.. code:: c

    yastr yaslRemoveFreeSpace(yastr s)

The :c:`yaslRemoveFreeSpace()` function :c:`realloc()`'s the string so that it
has no free space at the end. The contained string will be changed, but the
next concatenation operation will require an reallocation.

This function will :c:`realloc()` the string so all references to the original
:c:`yastr` should be treated as invalid and should be replaced with the one
returned by this function.

