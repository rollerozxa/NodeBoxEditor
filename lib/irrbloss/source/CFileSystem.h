// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_FILE_SYSTEM_H_INCLUDED__
#define __C_FILE_SYSTEM_H_INCLUDED__

#include "IFileSystem.h"
#include "irrArray.h"

namespace irr {
namespace io
{

/*!
	FileSystem which uses normal files and one zipfile
*/
class CFileSystem : public IFileSystem {
public:

	//! constructor
	CFileSystem();

	//! destructor
	virtual ~CFileSystem();

	//! opens a file for read access
	virtual IReadFile* createAndOpenFile(const io::path& filename) _IRR_OVERRIDE_;

	//! Creates an IReadFile interface for accessing memory like a file.
	virtual IReadFile* createMemoryReadFile(const void* memory, s32 len, const io::path& fileName, bool deleteMemoryWhenDropped = false) _IRR_OVERRIDE_;

	//! Opens a file for write access.
	virtual IWriteFile* createAndWriteFile(const io::path& filename, bool append=false) _IRR_OVERRIDE_;

	//! Returns the string of the current working directory
	virtual const io::path& getWorkingDirectory() _IRR_OVERRIDE_;

	//! Converts a relative path to an absolute (unique) path, resolving symbolic links
	virtual io::path getAbsolutePath(const io::path& filename) const _IRR_OVERRIDE_;

	//! Returns the directory a file is located in.
	/** \param filename: The file to get the directory from */
	virtual io::path getFileDir(const io::path& filename) const _IRR_OVERRIDE_;

	//! flatten a path and file name for example: "/you/me/../." becomes "/you"
	virtual io::path& flattenFilename( io::path& directory, const io::path& root = "/" ) const _IRR_OVERRIDE_;

	//! Get the relative filename, relative to the given directory
	virtual path getRelativeFilename(const path& filename, const path& directory) const _IRR_OVERRIDE_;

	virtual EFileSystemType setFileListSystem(EFileSystemType listType) _IRR_OVERRIDE_;

	//! determines if a file exists and would be able to be opened.
	virtual bool existFile(const io::path& filename) const _IRR_OVERRIDE_;

private:

	//! Currently used FileSystemType
	EFileSystemType FileSystemType;
	//! WorkingDirectory for Native and Virtual filesystems
	io::path WorkingDirectory [2];
	//! currently attached Archives
	core::array<IFileArchive*> FileArchives;
};


} // end namespace irr
} // end namespace io

#endif

