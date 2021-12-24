// Copyright (C) 2002-2012 Nikolaus Gebhardt/ Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FILE_ARCHIVE_H_INCLUDED__
#define __I_FILE_ARCHIVE_H_INCLUDED__

#include "IReadFile.h"
#include "IFileList.h"

namespace irr {

namespace io {

//! FileSystemType: which filesystem should be used for e.g. browsing
enum EFileSystemType {
	FILESYSTEM_NATIVE = 0,	// Native OS FileSystem
	FILESYSTEM_VIRTUAL	// Virtual FileSystem
};

//! Contains the different types of archives
enum E_FILE_ARCHIVE_TYPE {
	//! A virtual directory
	EFAT_FOLDER  = MAKE_IRR_ID('f','l','d','r'),

	//! The type of this archive is unknown
	EFAT_UNKNOWN = MAKE_IRR_ID('u','n','k','n')
};

//! The FileArchive manages archives and provides access to files inside them.
class IFileArchive : public virtual IReferenceCounted {
public:

	//! Opens a file based on its name
	/** Creates and returns a new IReadFile for a file in the archive.
	\param filename The file to open
	\return Returns A pointer to the created file on success,
	or 0 on failure. */
	virtual IReadFile* createAndOpenFile(const path& filename) =0;

	//! Opens a file based on its position in the file list.
	/** Creates and returns
	\param index The zero based index of the file.
	\return Returns a pointer to the created file on success, or 0 on failure. */
	virtual IReadFile* createAndOpenFile(u32 index) =0;

	//! Returns the complete file tree
	/** \return Returns the complete directory tree for the archive,
	including all files and folders */
	virtual const IFileList* getFileList() const =0;

	//! get the archive type
	virtual E_FILE_ARCHIVE_TYPE getType() const { return EFAT_UNKNOWN; }

	//! return the name (id) of the file Archive
	virtual const io::path& getArchiveName() const =0;

};

} // end namespace io
} // end namespace irr

#endif

