/*
tinyfiledialogs.c
Unique code file of tiny file dialogs library - tinyfd
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

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h> /* on old system try <sys/dir.h> instead */
#include <stdlib.h> /* for freebsd */
#endif				/* _WIN32 */

#include <stdio.h>

#include "tinyfiledialogs.h" /* not needed */

#define MAX_PATH_OR_CMD 1024 /* _MAX_PATH or MAX_PATH */
#define MAX_MULTIPLE 32

int tinyfd_forceConsole = 0; /* for UNIX only: 0 (default) or 1 */
/* 1 forces all dialogs into console mode even when the X server is present */
/* can be modified at run time */

static char *getPathWithoutFinalSlash(
	char *const aoDestination, /* make sure it is allocated, use _MAX_PATH */
	char const *const aSource) /* aoDestination and aSource can be the same */
{
	char const *lTmp;
	if (aSource) {
		lTmp = strrchr(aSource, (int)*"/");
		if (!lTmp) {
			lTmp = strrchr(aSource, (int)*"\\");
		}
		if (lTmp) {
			strncpy(aoDestination, aSource, lTmp - aSource);
			aoDestination[lTmp - aSource] = '\0';
		} else {
			*aoDestination = '\0';
		}
	} else {
		*aoDestination = '\0';
	}
	return aoDestination;
}

static char *getLastName(
	char *const aoDestination, /* make sure it is allocated */
	char const *const aSource)
{
	/* copy the last name after '/' or '\' */
	char const *lTmp;
	if (aSource) {
		lTmp = strrchr(aSource, (int)*"/");
		if (!lTmp) {
			lTmp = strrchr(aSource, (int)*"\\");
		}
		if (lTmp) {
			strcpy(aoDestination, lTmp + 1);
		} else {
			strcpy(aoDestination, aSource);
		}
	} else {
		*aoDestination = '\0';
	}
	return aoDestination;
}

static void Hex2RGB(char const aHexRGB[8],
					unsigned char aoResultRGB[3])
{
	char lColorChannel[8];
	strcpy(lColorChannel, aHexRGB);
	aoResultRGB[2] = (unsigned char)strtoul(lColorChannel + 5, NULL, 16);
	lColorChannel[5] = '\0';
	aoResultRGB[1] = (unsigned char)strtoul(lColorChannel + 3, NULL, 16);
	lColorChannel[3] = '\0';
	aoResultRGB[0] = (unsigned char)strtoul(lColorChannel + 1, NULL, 16);
	/* printf("%d %d %d\n", aoResultRGB[0], aoResultRGB[1], aoResultRGB[2]);//*/
}

static void RGB2Hex(unsigned char const aRGB[3],
					char aoResultHexRGB[8])
{
	sprintf(aoResultHexRGB, "#%02hhx%02hhx%02hhx", aRGB[0], aRGB[1], aRGB[2]);
	/* printf("aoResultHexRGB %s\n", aoResultHexRGB); //*/
}

#ifdef _WIN32

char const *tinyfd_saveFileDialog(
	char const *const aTitle,			   /* NULL or "" */
	char const *const aDefaultPathAndFile, /* NULL or "" */
	int const aNumOfFileFilters,		   /* 0 */
	char const *const *const aFileFilters) /* NULL or {"*.jpg","*.png"} */
{
	static char lBuff[MAX_PATH_OR_CMD];
	char lDirname[MAX_PATH_OR_CMD];
	char lFileFilters[MAX_PATH_OR_CMD] = "";
	char lString[MAX_PATH_OR_CMD];
	int i;
	char *p;
	OPENFILENAME ofn;

	getPathWithoutFinalSlash(lDirname, aDefaultPathAndFile);
	getLastName(lBuff, aDefaultPathAndFile);

	if (aNumOfFileFilters > 0) {
		strcat(lFileFilters, aFileFilters[0]);
		for (i = 1; i < aNumOfFileFilters; i++) {
			strcat(lFileFilters, ";");
			strcat(lFileFilters, aFileFilters[i]);
		}
		strcat(lFileFilters, "\n");
		strcpy(lString, lFileFilters);
		strcat(lFileFilters, lString);
		strcat(lFileFilters, "All Files\n*.*\n");
		p = lFileFilters;
		while ((p = strchr(p, '\n')) != NULL) {
			*p = '\0';
			p++;
		}
	}

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = 0;
	ofn.hInstance = 0;
	ofn.lpstrFilter = lFileFilters;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = lBuff;
	ofn.nMaxFile = MAX_PATH_OR_CMD;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
	ofn.lpstrInitialDir = lDirname;
	ofn.lpstrTitle = aTitle;
	ofn.Flags = OFN_OVERWRITEPROMPT;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = NULL;
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
	ofn.Flags |= OFN_NOCHANGEDIR;

	if (GetSaveFileName(&ofn) == 0) {
		return NULL;
	} else {
		return lBuff;
	}
}

/* in case of multiple files, the separator is | */
char const *tinyfd_openFileDialog(
	char const *const aTitle,			   /*  NULL or "" */
	char const *const aDefaultPathAndFile, /*  NULL or "" */
	int const aNumOfFileFilters,		   /* 0 */
	char const *const *const aFileFilters, /* NULL or {"*.jpg","*.png"} */
	int aAllowMultipleSelects)			   /* 0 or 1 */
{
	static char lBuff[MAX_MULTIPLE * MAX_PATH_OR_CMD];
	char lDirname[MAX_PATH_OR_CMD];
	char lFileFilters[MAX_PATH_OR_CMD] = "";
	char lTempString[MAX_PATH_OR_CMD];
	char *lPointers[MAX_MULTIPLE];
	size_t lLengths[MAX_MULTIPLE];
	int i, j;
	char *p;
	OPENFILENAME ofn;
	size_t lBuffLen;

	getPathWithoutFinalSlash(lDirname, aDefaultPathAndFile);
	getLastName(lBuff, aDefaultPathAndFile);

	if (aNumOfFileFilters > 0) {
		strcat(lFileFilters, aFileFilters[0]);
		for (i = 1; i < aNumOfFileFilters; i++) {
			strcat(lFileFilters, ";");
			strcat(lFileFilters, aFileFilters[i]);
		}
		strcat(lFileFilters, "\n");
		strcpy(lTempString, lFileFilters);
		strcat(lFileFilters, lTempString);
		strcat(lFileFilters, "All Files\n*.*\n");
		p = lFileFilters;
		while ((p = strchr(p, '\n')) != NULL) {
			*p = '\0';
			p++;
		}
	}

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = 0;
	ofn.hInstance = 0;
	ofn.lpstrFilter = lFileFilters;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = lBuff;
	ofn.nMaxFile = MAX_PATH_OR_CMD;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
	ofn.lpstrInitialDir = lDirname;
	ofn.lpstrTitle = aTitle;
	ofn.Flags = OFN_EXPLORER;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = NULL;
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;

	if (aAllowMultipleSelects) {
		ofn.Flags |= OFN_ALLOWMULTISELECT;
	}

	ofn.Flags |= OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn) == 0) {
		return NULL;
	} else {
		lBuffLen = strlen(lBuff);
		lPointers[0] = lBuff + lBuffLen + 1;
		if (!aAllowMultipleSelects || (lPointers[0][0] == '\0'))
			return lBuff;

		i = 0;
		do {
			lLengths[i] = strlen(lPointers[i]);
			lPointers[i + 1] = lPointers[i] + lLengths[i] + 1;
			i++;
		} while (lPointers[i][0] != '\0');
		i--;
		p = lBuff + sizeof(lBuff) - 1;
		*p = '\0';
		for (j = i; j >= 0; j--) {
			p -= lLengths[j];
			memcpy(p, lPointers[j], lLengths[j]);
			p--;
			*p = '\\';
			p -= lBuffLen;
			memcpy(p, lBuff, lBuffLen);
			p--;
			*p = '|';
		}
		p++;
		return p;
	}
}

char const *tinyfd_selectFolderDialog(
	char const *const aTitle,		/*  NULL or "" */
	char const *const aDefaultPath) /* NULL or "" */
{
	static char lBuff[MAX_PATH_OR_CMD];
	BROWSEINFO bInfo;
	LPITEMIDLIST lpItem;

	/* we can't use aDefaultPath */
	bInfo.hwndOwner = 0;
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = lBuff;
	bInfo.lpszTitle = aTitle;
	bInfo.ulFlags = 0;
	bInfo.lpfn = NULL;
	bInfo.lParam = 0;
	bInfo.iImage = -1;

	lpItem = SHBrowseForFolder(&bInfo);
	if (lpItem) {
		SHGetPathFromIDList(lpItem, lBuff);
	}

	return lBuff;
}

#else /* unix */

static char gTitle[] = "missing software !";

static char gMessage[] = "tiny file dialogs on UNIX needs zenity or kdialog\n";

static int graphicMode()
{
	return (!tinyfd_forceConsole) && getenv("DISPLAY");
}

static int detectPresence(char const *const aExecutable)
{
	FILE *lIn;
	char lBuff[MAX_PATH_OR_CMD];
	char lTestedString[MAX_PATH_OR_CMD] = "which ";
	strcat(lTestedString, aExecutable);
	lIn = popen(lTestedString, "r");
	if ((fgets(lBuff, sizeof(lBuff), lIn) != NULL) && (!strchr(lBuff, ':'))) {
		/* present */
		pclose(lIn);
		return 1;
	} else {
		pclose(lIn);
		return 0;
	}
}

static int tryCommand(char const *const aCommand)
{
	char lBuff[MAX_PATH_OR_CMD];
	FILE *lIn = popen(aCommand, "r");
	if (fgets(lBuff, sizeof(lBuff), lIn) == NULL) {
		/* present */
		pclose(lIn);
		return 1;
	} else {
		pclose(lIn);
		return 0;
	}
}

static int kdialogPresent()
{
	static int lKdialogPresent = -1;
	if (lKdialogPresent < 0) {
		lKdialogPresent = detectPresence("kdialog");
	}
	return lKdialogPresent && graphicMode();
}

static int zenityPresent()
{
	static int lZenityPresent = -1;
	if (lZenityPresent < 0) {
		lZenityPresent = detectPresence("zenity");
	}
	return lZenityPresent && graphicMode();
}

static void replaceSubStr(char const *const aSource,
						  char const *const aOldSubStr,
						  char const *const aNewSubStr,
						  char *const aoDestination)
{
	char const *pOccurence;
	char const *p;
	char const *lNewSubStr = "";

	if (!aSource) {
		*aoDestination = '\0';
		return;
	}
	if (!aOldSubStr) {
		strcpy(aoDestination, aSource);
		return;
	}
	if (aNewSubStr) {
		lNewSubStr = aNewSubStr;
	}
	p = aSource;
	int lOldSubLen = strlen(aOldSubStr);
	*aoDestination = '\0';
	while ((pOccurence = strstr(p, aOldSubStr)) != NULL) {
		strncat(aoDestination, p, pOccurence - p);
		strcat(aoDestination, lNewSubStr);
		p = pOccurence + lOldSubLen;
	}
	strcat(aoDestination, p);
}

char const *tinyfd_saveFileDialog(
	char const *const aTitle,			   /* NULL or "" */
	char const *const aDefaultPathAndFile, /* NULL or "" */
	int const aNumOfFileFilters,		   /* 0 */
	char const *const *const aFileFilters) /* NULL or {"*.jpg","*.png"} */
{
	static char lBuff[MAX_PATH_OR_CMD];
	char lDialogString[MAX_PATH_OR_CMD];
	char lString[MAX_PATH_OR_CMD];
	int i;
	DIR *lDir;
	FILE *lIn;
	lBuff[0] = '\0';

	if (zenityPresent()) {
		printf("using zenity for saveFileDialog\n");
		strcpy(lDialogString,
			   "zenity --file-selection --save --confirm-overwrite");
		if (aTitle && strlen(aTitle)) {
			strcat(lDialogString, " --title=\"");
			strcat(lDialogString, aTitle);
			strcat(lDialogString, "\"");
		}
		if (aDefaultPathAndFile && strlen(aDefaultPathAndFile)) {
			strcat(lDialogString, " --filename=\"");
			strcat(lDialogString, aDefaultPathAndFile);
			strcat(lDialogString, "\"");
		}
		if (aNumOfFileFilters > 0) {
			for (i = 0; i < aNumOfFileFilters; i++) {
				strcat(lDialogString, " --file-filter='");
				strcat(lDialogString, aFileFilters[i]);
				strcat(lDialogString, "'");
			}
			strcat(lDialogString, " --file-filter='All files | *'");
		}
	} else if (kdialogPresent()) {
		printf("using kdialog for saveFileDialog\n");
		strcpy(lDialogString, "kdialog --getsavefilename");
		if (aDefaultPathAndFile && strlen(aDefaultPathAndFile)) {
			strcat(lDialogString, " \"");
			strcat(lDialogString, aDefaultPathAndFile);
			strcat(lDialogString, "\"");
		} else {
			strcat(lDialogString, " :");
		}
		if (aNumOfFileFilters > 0) {
			strcat(lDialogString, " \"");
			for (i = 0; i < aNumOfFileFilters; i++) {
				strcat(lDialogString, aFileFilters[i]);
				strcat(lDialogString, " ");
			}
			strcat(lDialogString, "\"");
		}
		if (aTitle && strlen(aTitle)) {
			strcat(lDialogString, " --title \"");
			strcat(lDialogString, aTitle);
			strcat(lDialogString, "\"");
		}
	}
	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	if (!(lIn = popen(lDialogString, "r"))) {
		return NULL;
	}
	while (fgets(lBuff, sizeof(lBuff), lIn) != NULL) {
	}
	pclose(lIn);
	if (lBuff[strlen(lBuff) - 1] == '\n') {
		lBuff[strlen(lBuff) - 1] = '\0';
	}
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	getPathWithoutFinalSlash(lString, lBuff);
	if (strlen(lString) > 0) {
		lDir = opendir(lString);
		if (!lDir) {
			return NULL;
		}
		closedir(lDir);
	}
	return lBuff;
}

/* in case of multiple files, the separator is | */
char const *tinyfd_openFileDialog(
	char const *const aTitle,			   /* NULL or "" */
	char const *const aDefaultPathAndFile, /* NULL or "" */
	int const aNumOfFileFilters,		   /* 0 */
	char const *const *const aFileFilters, /* NULL or {"*.jpg","*.png"} */
	int aAllowMultipleSelects)			   /* 0 or 1 */
{
	static char lBuff[MAX_MULTIPLE * MAX_PATH_OR_CMD];
	char lDialogString[MAX_PATH_OR_CMD];
	char lString[MAX_PATH_OR_CMD];
	int i;
	FILE *lIn;
	char *p;
	int lWasKdialog = 0;
	lBuff[0] = '\0';

	if (zenityPresent()) {
		strcpy(lDialogString, "zenity --file-selection");
		if (aAllowMultipleSelects) {
			strcat(lDialogString, " --multiple");
		}
		if (aTitle && strlen(aTitle)) {
			strcat(lDialogString, " --title=\"");
			strcat(lDialogString, aTitle);
			strcat(lDialogString, "\"");
		}
		if (aDefaultPathAndFile && strlen(aDefaultPathAndFile)) {
			strcat(lDialogString, " --filename=\"");
			strcat(lDialogString, aDefaultPathAndFile);
			strcat(lDialogString, "\"");
		}
		if (aNumOfFileFilters > 0) {
			strcat(lDialogString, " --file-filter='");
			for (i = 0; i < aNumOfFileFilters; i++) {
				strcat(lDialogString, aFileFilters[i]);
				strcat(lDialogString, " ");
			}
			strcat(lDialogString, "' --file-filter='All files | *'");
		}
	} else if (kdialogPresent()) {
		lWasKdialog = 1;
		strcpy(lDialogString, "kdialog --getopenfilename");
		if (aDefaultPathAndFile && strlen(aDefaultPathAndFile)) {
			strcat(lDialogString, " \"");
			strcat(lDialogString, aDefaultPathAndFile);
			strcat(lDialogString, "\"");
		} else {
			strcat(lDialogString, " :");
		}
		if (aNumOfFileFilters > 0) {
			strcat(lDialogString, " \"");
			for (i = 0; i < aNumOfFileFilters; i++) {
				strcat(lDialogString, aFileFilters[i]);
				strcat(lDialogString, " ");
			}
			strcat(lDialogString, "\"");
		}
		if (aAllowMultipleSelects) {
			strcat(lDialogString, " --multiple --separate-output");
		}
		if (aTitle && strlen(aTitle)) {
			strcat(lDialogString, " --title \"");
			strcat(lDialogString, aTitle);
			strcat(lDialogString, "\"");
		}
	}
	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	if (!(lIn = popen(lDialogString, "r"))) {
		return NULL;
	}
	lBuff[0] = '\0';
	p = lBuff;
	while (fgets(p, sizeof(lBuff), lIn) != NULL) {
		p += strlen(p);
	}
	pclose(lIn);
	if (lBuff[strlen(lBuff) - 1] == '\n') {
		lBuff[strlen(lBuff) - 1] = '\0';
	}
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	if (lWasKdialog && aAllowMultipleSelects) {
		p = lBuff;
		while ((p = strchr(p, '\n')))
			*p = '|';
	}
	/* printf ( "lBuff2: %s\n" , lBuff ) ; //*/
	if (!aAllowMultipleSelects) {
		lIn = fopen(lBuff, "r");
		if (!lIn) {
			return NULL;
		}
		fclose(lIn);
	}
	/* printf ( "lBuff3: %s\n" , lBuff ) ; //*/
	return lBuff;
}

char const *tinyfd_selectFolderDialog(
	char const *const aTitle,		/* "" */
	char const *const aDefaultPath) /* "" */
{
	static char lBuff[MAX_PATH_OR_CMD];
	char lDialogString[MAX_PATH_OR_CMD];
	DIR *lDir;
	FILE *lIn;
	lBuff[0] = '\0';

	if (zenityPresent()) {
		strcpy(lDialogString, "zenity --file-selection --directory");
		if (aTitle && strlen(aTitle)) {
			strcat(lDialogString, " --title=\"");
			strcat(lDialogString, aTitle);
			strcat(lDialogString, "\"");
		}
		if (aDefaultPath && strlen(aDefaultPath)) {
			strcat(lDialogString, " --filename=\"");
			strcat(lDialogString, aDefaultPath);
			strcat(lDialogString, "\"");
		}
	}
	else if (kdialogPresent()) {
		strcpy(lDialogString, "kdialog --getexistingdirectory");
		if (aDefaultPath && strlen(aDefaultPath)) {
			strcat(lDialogString, " \"");
			strcat(lDialogString, aDefaultPath);
			strcat(lDialogString, "\"");
		} else {
			strcat(lDialogString, " :");
		}
		if (aTitle && strlen(aTitle)) {
			strcat(lDialogString, " --title \"");
			strcat(lDialogString, aTitle);
			strcat(lDialogString, "\"");
		}
	}
	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	if (!(lIn = popen(lDialogString, "r"))) {
		return NULL;
	}
	while (fgets(lBuff, sizeof(lBuff), lIn) != NULL) {
	}
	pclose(lIn);
	if (lBuff[strlen(lBuff) - 1] == '\n') {
		lBuff[strlen(lBuff) - 1] = '\0';
	}
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	if (strlen(lBuff) > 0) {
		lDir = opendir(lBuff);
		if (!lDir) {
			return NULL;
		}
		closedir(lDir);
	}
	return lBuff;
}

#endif /* _WIN32 */
