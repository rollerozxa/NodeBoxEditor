/*
tinyfiledialogs.h
optional unique header file of the tiny file dialogs library - tinyfd
created [November 9, 2014]
Copyright (c) 2014 - 2015 Guillaume Vareille http://ysengrin.com
http://tinyfiledialogs.sourceforge.net

- License -

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.  If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef TINYFILEDIALOGS_H
#define TINYFILEDIALOGS_H

/*
if tinydialogs.c is compiled with a C++ compiler
rather than with a C compiler, you need to comment out:
extern "C" {
and the corresponding closing bracket:
}
*/

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

extern int tinyfd_forceConsole ; /* for UNIX only: 0 (default) or 1 */
/* 1 forces all dialogs into console mode even when the X server is present */
/* can be modified at run time */

char const * tinyfd_saveFileDialog (
    char const * const aTitle , /* "" */
    char const * const aDefaultPathAndFile , /* "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters ) ; /* NULL or {"*.txt"} */

char const * tinyfd_openFileDialog (
    char const * const aTitle , /* "" */
    char const * const aDefaultPathAndFile , /* "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters , /* NULL or {"*.jpg","*.png"} */
    int aAllowMultipleSelects ) ; /* 0 or 1 */
/* in case of multiple files, the separator is | */

char const * tinyfd_selectFolderDialog (
	char const * const aTitle , /* "" */
    char const * const aDefaultPath ) ; /* "" */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* TINYFILEDIALOGS_H */
