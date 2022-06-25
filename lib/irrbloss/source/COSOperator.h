// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OS_OPERATOR_H_INCLUDED__
#define __C_OS_OPERATOR_H_INCLUDED__

#include "IOSOperator.h"

namespace irr {

class CIrrDeviceLinux;

//! The Operating system operator provides operation system specific methods and information.
class COSOperator : public IOSOperator {
public:

	// constructor
#if defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	COSOperator(const core::stringc& osversion, CIrrDeviceLinux* device);
#endif
	COSOperator(const core::stringc& osversion);

	//! returns the current operation system version as string.
	virtual const core::stringc& getOperatingSystemVersion() const _IRR_OVERRIDE_;

	//! copies text to the clipboard
	virtual void copyToClipboard(const c8* text) const _IRR_OVERRIDE_;

	//! gets text from the clipboard
	//! \return Returns 0 if no string is in there.
	virtual const c8* getTextFromClipboard() const _IRR_OVERRIDE_;

private:

	core::stringc OperatingSystem;

#if defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	CIrrDeviceLinux * IrrDeviceLinux;
#endif

};

} // end namespace

#endif
