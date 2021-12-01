// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_OS_OPERATOR_H_INCLUDED__
#define __I_OS_OPERATOR_H_INCLUDED__

#include "IReferenceCounted.h"
#include "irrString.h"

namespace irr {

//! The Operating system operator provides operation system specific methods and information.
class IOSOperator : public virtual IReferenceCounted {
public:
	//! Get the current operation system version as string.
	virtual const core::stringc& getOperatingSystemVersion() const = 0;

	//! Copies text to the clipboard
	virtual void copyToClipboard(const c8* text) const = 0;

	//! Get text from the clipboard
	/** \return Returns 0 if no string is in there. */
	virtual const c8* getTextFromClipboard() const = 0;
};

} // end namespace

#endif
