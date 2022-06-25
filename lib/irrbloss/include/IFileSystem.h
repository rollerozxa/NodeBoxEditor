// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FILE_SYSTEM_H_INCLUDED__
#define __I_FILE_SYSTEM_H_INCLUDED__

#include "IReferenceCounted.h"
#include "IFileArchive.h"

namespace irr {
namespace video
{
	class IVideoDriver;
} // end namespace video
namespace io {

class IReadFile;
class IWriteFile;
class IFileList;


//! The FileSystem manages files and archives and provides access to them.
/** It manages where files are, so that modules which use the the IO do not
need to know where every file is located. A file could be in a .zip-Archive or
as file on disk, using the IFileSystem makes no difference to this. */
class IFileSystem : public virtual IReferenceCounted {
public:

	//! Opens a file for read access.
	/** \param filename: Name of file to open.
	\return Pointer to the created file interface.
	The returned pointer should be dropped when no longer needed.
	See IReferenceCounted::drop() for more information. */
	virtual IReadFile* createAndOpenFile(const path& filename) =0;

	//! Creates an IReadFile interface for accessing memory like a file.
	/** This allows you to use a pointer to memory where an IReadFile is requested.
	\param memory: A pointer to the start of the file in memory
	\param len: The length of the memory in bytes
	\param fileName: The name given to this file
	\param deleteMemoryWhenDropped: True if the memory should be deleted
	along with the IReadFile when it is dropped.
	\return Pointer to the created file interface.
	The returned pointer should be dropped when no longer needed.
	See IReferenceCounted::drop() for more information.
	*/
	virtual IReadFile* createMemoryReadFile(const void* memory, s32 len, const path& fileName, bool deleteMemoryWhenDropped=false) =0;

	//! Opens a file for write access.
	/** \param filename: Name of file to open.
	\param append: If the file already exist, all write operations are
	appended to the file.
	\return Pointer to the created file interface. 0 is returned, if the
	file could not created or opened for writing.
	The returned pointer should be dropped when no longer needed.
	See IReferenceCounted::drop() for more information. */
	virtual IWriteFile* createAndWriteFile(const path& filename, bool append=false) =0;

	//! Get the current working directory.
	/** \return Current working directory as a string. */
	virtual const path& getWorkingDirectory() =0;

	//! Converts a relative path to an absolute (unique) path, resolving symbolic links if required
	/** \param filename Possibly relative file or directory name to query.
	\result Absolute filename which points to the same file. */
	virtual path getAbsolutePath(const path& filename) const =0;

	//! Get the directory a file is located in.
	/** \param filename: The file to get the directory from.
	\return String containing the directory of the file. */
	virtual path getFileDir(const path& filename) const =0;

	//! flatten a path and file name for example: "/you/me/../." becomes "/you"
	virtual path& flattenFilename(path& directory, const path& root="/") const =0;

	//! Get the relative filename, relative to the given directory
	virtual path getRelativeFilename(const path& filename, const path& directory) const =0;

	//! Set the active type of file system.
	virtual EFileSystemType setFileListSystem(EFileSystemType listType) =0;

	//! Determines if a file exists and could be opened.
	/** \param filename is the string identifying the file which should be tested for existence.
	\return True if file exists, and false if it does not exist or an error occurred. */
	virtual bool existFile(const path& filename) const =0;
};


} // end namespace io
} // end namespace irr

#endif

