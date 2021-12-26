// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COSOperator.h"

#ifdef _IRR_WINDOWS_API_
#ifndef _IRR_XBOX_PLATFORM_
#include <windows.h>
#endif
#else
#include <string.h>
#include <unistd.h>
#ifndef _IRR_ANDROID_PLATFORM_
#include <sys/types.h>
#endif
#endif

#if defined(_IRR_COMPILE_WITH_X11_DEVICE_)
#include "CIrrDeviceLinux.h"
#endif

#include "fast_atof.h"

namespace irr {

#if defined(_IRR_COMPILE_WITH_X11_DEVICE_)
// constructor  linux
	COSOperator::COSOperator(const core::stringc& osVersion, CIrrDeviceLinux* device)
: OperatingSystem(osVersion), IrrDeviceLinux(device) {
}
#endif

// constructor
COSOperator::COSOperator(const core::stringc& osVersion) : OperatingSystem(osVersion) {
	#ifdef _DEBUG
	setDebugName("COSOperator");
	#endif
}

//! returns the current operating system version as string.
const core::stringc& COSOperator::getOperatingSystemVersion() const {
	return OperatingSystem;
}

//! copies text to the clipboard
void COSOperator::copyToClipboard(const c8* text) const {
	if (strlen(text)==0)
		return;

// Windows version
#if defined(_IRR_WINDOWS_API_)
	if (!OpenClipboard(NULL) || text == 0)
		return;

	EmptyClipboard();

	HGLOBAL clipbuffer;
	char * buffer;

	clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(text)+1);
	buffer = (char*)GlobalLock(clipbuffer);

	strcpy(buffer, text);

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	if ( IrrDeviceLinux )
		IrrDeviceLinux->copyToClipboard(text);
#endif
}

//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
const c8* COSOperator::getTextFromClipboard() const {
#if defined(_IRR_WINDOWS_API_)
	if (!OpenClipboard(NULL))
		return 0;

	char * buffer = 0;

	HANDLE hData = GetClipboardData( CF_TEXT );
	buffer = (char*)GlobalLock( hData );
	GlobalUnlock( hData );
	CloseClipboard();
	return buffer;
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	if ( IrrDeviceLinux )
		return IrrDeviceLinux->getTextFromClipboard();
	return 0;
#else
	return 0;
#endif
}

} // end namespace

