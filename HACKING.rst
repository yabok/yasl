Function ordering
=================

The functions are ordered in groups according to what they do, and the groups
are signified in the source code using C99 ``// ... //`` line comments. The
following groups exist:

1. Initialization
2. Querying
3. Modification
4. Concatenation
5. Freeing
6. Low-level functions

Note: the Concatenation group technically belongs to the Modification group,
but they were split out due to the amount of them.

Commit messages
===============

A commit message should have a short, roughly 50 characters or shorter summary
describing what was done in the commit. The summary is optionally followed by a
longer description of the changes and reasons for them after a blank line, and
wrapped at around 72 characters.

If the commit fixes a bug that has been previously reported, include a line
like ``Fixes #123`` in the commit message. When the commit is merged it will
cause the issue to automatically be closed. See the GitHub help page on
`Closing issues via commit messages`_ for a list of all keywords.

All commits contributed should also contain a sign-off line to show that you
accept the `Developer Certificate of Origin`_. Add a sign-off line by adding a
line like the following to the end of each of your commit messages::

    Signed-off-by: Random J Developer <random@developer.example.org>


References: `A Note About Git Commit Messages`_

.. _`Closing issues via commit messages`:
    https://help.github.com/articles/closing-issues-via-commit-messages/
.. _`Developer Certificate of Origin`: http://developercertificate.org/
.. _`A Note About Git Commit Messages`:
    http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html

Style guide
===========

First of all note that this is a style *guide* and not hard-set rules.

Line length
-----------

Lines should *preferably* be under 80 if possible, but longer lines are allowed
or even encouraged if either necessary or if it makes the code nicer and cleaner.

Indentation
-----------

All source code is indented using tabs and aligned with spaces. A tab is
counted as 4 characters when determining the length of a line.
