// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#include "CFileSystem.h"
#include "IReadFile.h"
#include "IWriteFile.h"
#include "CFileList.h"
#include "stdio.h"
#include "os.h"
#include "CReadFile.h"
#include "CMemoryFile.h"
#include "CWriteFile.h"
#include "irrList.h"

#if defined (__STRICT_ANSI__)
    #error Compiling with __STRICT_ANSI__ not supported. g++ does set this when compiling with -std=c++11 or -std=c++0x. Use instead -std=gnu++11 or -std=gnu++0x. Or use -U__STRICT_ANSI__ to disable strict ansi.
#endif

#if defined (_IRR_WINDOWS_API_)
	#if !defined ( _WIN32_WCE )
		#include <direct.h> // for _chdir
		#include <io.h> // for _access
		#include <tchar.h>
	#endif
#elif defined(_IRR_POSIX_API_)
		#include <stdio.h>
		#include <stdlib.h>
		#include <string.h>
		#include <limits.h>
		#include <sys/types.h>
		#include <dirent.h>
		#include <sys/stat.h>
		#include <unistd.h>
#endif

namespace irr {
namespace io
{

//! constructor
CFileSystem::CFileSystem() {
	#ifdef _DEBUG
	setDebugName("CFileSystem");
	#endif

	setFileListSystem(FILESYSTEM_NATIVE);
	//! reset current working directory
	getWorkingDirectory();
}


//! destructor
CFileSystem::~CFileSystem() {
	u32 i;

	for ( i=0; i < FileArchives.size(); ++i) {
		FileArchives[i]->drop();
	}
}


//! opens a file for read access
IReadFile* CFileSystem::createAndOpenFile(const io::path& filename) {
	if ( filename.empty() )
		return 0;

	IReadFile* file = 0;
	u32 i;

	for (i=0; i< FileArchives.size(); ++i) {
		file = FileArchives[i]->createAndOpenFile(filename);
		if (file)
			return file;
	}

	// Create the file using an absolute path so that it matches
	// the scheme used by CNullDriver::getTexture().
	return CReadFile::createReadFile(getAbsolutePath(filename));
}


//! Creates an IReadFile interface for treating memory like a file.
IReadFile* CFileSystem::createMemoryReadFile(const void* memory, s32 len,
		const io::path& fileName, bool deleteMemoryWhenDropped) {
	if (!memory)
		return 0;
	else
		return new CMemoryReadFile(memory, len, fileName, deleteMemoryWhenDropped);
}


//! Opens a file for write access.
IWriteFile* CFileSystem::createAndWriteFile(const io::path& filename, bool append) {
	return CWriteFile::createWriteFile(filename, append);
}

//! Returns the string of the current working directory
const io::path& CFileSystem::getWorkingDirectory() {
	EFileSystemType type = FileSystemType;

	if (type != FILESYSTEM_NATIVE) {
		type = FILESYSTEM_VIRTUAL;
	} else {
		#if defined(_IRR_WINDOWS_API_)
			fschar_t tmp[_MAX_PATH];
			_getcwd(tmp, _MAX_PATH);
			WorkingDirectory[FILESYSTEM_NATIVE] = tmp;
			WorkingDirectory[FILESYSTEM_NATIVE].replace('\\', '/');
		#endif

		#if defined(_IRR_POSIX_API_)
			// getting the CWD is rather complex as we do not know the size
			// so try it until the call was successful
			// Note that neither the first nor the second parameter may be 0 according to POSIX

			u32 pathSize=256;
			char *tmpPath = new char[pathSize];
			while ((pathSize < (1<<16)) && !(getcwd(tmpPath,pathSize))) {
				delete [] tmpPath;
				pathSize *= 2;
				tmpPath = new char[pathSize];
			}
			if (tmpPath) {
				WorkingDirectory[FILESYSTEM_NATIVE] = tmpPath;
				delete [] tmpPath;
			}
		#endif

		WorkingDirectory[type].validate();
	}

	return WorkingDirectory[type];
}

io::path CFileSystem::getAbsolutePath(const io::path& filename) const {
	if ( filename.empty() )
		return filename;
#if defined(_IRR_WINDOWS_API_)
	fschar_t *p=0;
	fschar_t fpath[_MAX_PATH];
	p = _fullpath(fpath, filename.c_str(), _MAX_PATH);
	core::stringc tmp(p);
	tmp.replace('\\', '/');
	return tmp;
#elif defined(_IRR_POSIX_API_)
	c8* p=0;
	c8 fpath[4096];
	fpath[0]=0;
	p = realpath(filename.c_str(), fpath);
	if (!p) {
		// content in fpath is unclear at this point
		if (!fpath[0]) { // seems like fpath wasn't altered, use our best guess
			io::path tmp(filename);
			return flattenFilename(tmp);
		}
		else
			return io::path(fpath);
	}
	if (filename[filename.size()-1]=='/')
		return io::path(p)+_IRR_TEXT("/");
	else
		return io::path(p);
#else
	return io::path(filename);
#endif
}


//! returns the directory part of a filename, i.e. all until the first
//! slash or backslash, excluding it. If no directory path is prefixed, a '.'
//! is returned.
io::path CFileSystem::getFileDir(const io::path& filename) const {
	// find last forward or backslash
	s32 lastSlash = filename.findLast('/');
	const s32 lastBackSlash = filename.findLast('\\');
	lastSlash = lastSlash > lastBackSlash ? lastSlash : lastBackSlash;

	if ((u32)lastSlash < filename.size())
		return filename.subString(0, lastSlash);
	else
		return _IRR_TEXT(".");
}


//! flatten a path and file name for example: "/you/me/../." becomes "/you"
io::path& CFileSystem::flattenFilename(io::path& directory, const io::path& root) const {
	directory.replace('\\', '/');
	if (directory.lastChar() != '/')
		directory.append('/');

	io::path dir;
	io::path subdir;

	s32 lastpos = 0;
	s32 pos = 0;
	bool lastWasRealDir=false;

	while ((pos = directory.findNext('/', lastpos)) >= 0) {
		subdir = directory.subString(lastpos, pos - lastpos + 1);

		if (subdir == _IRR_TEXT("../")) {
			if (lastWasRealDir) {
				deletePathFromPath(dir, 2);
				lastWasRealDir=(dir.size()!=0);
			} else {
				dir.append(subdir);
				lastWasRealDir=false;
			}
		}
		else if (subdir == _IRR_TEXT("/")) {
			dir = root;
		}
		else if (subdir != _IRR_TEXT("./")) {
			dir.append(subdir);
			lastWasRealDir=true;
		}

		lastpos = pos + 1;
	}
	directory = dir;
	return directory;
}


//! Get the relative filename, relative to the given directory
path CFileSystem::getRelativeFilename(const path& filename, const path& directory) const {
	if ( filename.empty() || directory.empty() )
		return filename;

	io::path path1, file, ext;
	core::splitFilename(getAbsolutePath(filename), &path1, &file, &ext);
	io::path path2(getAbsolutePath(directory));
	core::list<io::path> list1, list2;
	path1.split(list1, _IRR_TEXT("/\\"), 2);
	path2.split(list2, _IRR_TEXT("/\\"), 2);
	u32 i=0;
	core::list<io::path>::ConstIterator it1,it2;
	it1=list1.begin();
	it2=list2.begin();

	#if defined (_IRR_WINDOWS_API_)
	fschar_t partition1 = 0, partition2 = 0;
	io::path prefix1, prefix2;
	if ( it1 != list1.end() )
		prefix1 = *it1;
	if ( it2 != list2.end() )
		prefix2 = *it2;
	if ( prefix1.size() > 1 && prefix1[1] == _IRR_TEXT(':') )
		partition1 = core::locale_lower(prefix1[0]);
	if ( prefix2.size() > 1 && prefix2[1] == _IRR_TEXT(':') )
		partition2 = core::locale_lower(prefix2[0]);

	// must have the same prefix or we can't resolve it to a relative filename
	if ( partition1 != partition2 ) {
		return filename;
	}
	#endif


	for (; i<list1.size() && i<list2.size()
#if defined (_IRR_WINDOWS_API_)
		&& (io::path(*it1).make_lower()==io::path(*it2).make_lower())
#else
		&& (*it1==*it2)
#endif
		; ++i) {
		++it1;
		++it2;
	}
	path1=_IRR_TEXT("");
	for (; i<list2.size(); ++i)
		path1 += _IRR_TEXT("../");
	while (it1 != list1.end()) {
		path1 += *it1++;
		path1 += _IRR_TEXT('/');
	}
	path1 += file;
	if (ext.size()) {
		path1 += _IRR_TEXT('.');
		path1 += ext;
	}
	return path1;
}


//! Sets the current file systen type
EFileSystemType CFileSystem::setFileListSystem(EFileSystemType listType) {
	EFileSystemType current = FileSystemType;
	FileSystemType = listType;
	return current;
}


//! determines if a file exists and would be able to be opened.
bool CFileSystem::existFile(const io::path& filename) const {
	for (u32 i=0; i < FileArchives.size(); ++i)
		if (FileArchives[i]->getFileList()->findFile(filename)!=-1)
			return true;

#if defined(_MSC_VER)
	return (_access(filename.c_str(), 0) != -1);
#elif defined(F_OK)
	return (access(filename.c_str(), F_OK) != -1);
#else
	return (access(filename.c_str(), 0) != -1);
#endif
}


//! creates a filesystem which is able to open files from the ordinary file system,
//! and out of zipfiles, which are able to be added to the filesystem.
IFileSystem* createFileSystem() {
	return new CFileSystem();
}


} // end namespace irr
} // end namespace io
