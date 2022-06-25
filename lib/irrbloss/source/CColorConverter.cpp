// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CColorConverter.h"
#include "SColor.h"
#include "os.h"
#include "irrString.h"

namespace irr {
namespace video {


void CColorConverter::convert_A1R5G5B5toR8G8B8(const void* sP, s32 sN, void* dP) {
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	for (s32 x = 0; x < sN; ++x) {
		dB[2] = (*sB & 0x7c00) >> 7;
		dB[1] = (*sB & 0x03e0) >> 2;
		dB[0] = (*sB & 0x1f) << 3;

		sB += 1;
		dB += 3;
	}
}


void CColorConverter::convert_A1R5G5B5toA8R8G8B8(const void* sP, s32 sN, void* dP) {
	u16* sB = (u16*)sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A1R5G5B5toA8R8G8B8(*sB++);
}

void CColorConverter::convert_A1R5G5B5toA1R5G5B5(const void* sP, s32 sN, void* dP) {
	memcpy(dP, sP, sN * 2);
}

void CColorConverter::convert_A1R5G5B5toR5G6B5(const void* sP, s32 sN, void* dP) {
	u16* sB = (u16*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A1R5G5B5toR5G6B5(*sB++);
}

void CColorConverter::convert_A8R8G8B8toR8G8B8(const void* sP, s32 sN, void* dP) {
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	for (s32 x = 0; x < sN; ++x) {
		// sB[3] is alpha
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];

		sB += 4;
		dB += 3;
	}
}

void CColorConverter::convert_A8R8G8B8toB8G8R8(const void* sP, s32 sN, void* dP) {
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	for (s32 x = 0; x < sN; ++x) {
		// sB[3] is alpha
		dB[0] = sB[0];
		dB[1] = sB[1];
		dB[2] = sB[2];

		sB += 4;
		dB += 3;
	}
}

void CColorConverter::convert_A8R8G8B8toA8R8G8B8(const void* sP, s32 sN, void* dP) {
	memcpy(dP, sP, sN * 4);
}

void CColorConverter::convert_A8R8G8B8toA1R5G5B5(const void* sP, s32 sN, void* dP) {
	u32* sB = (u32*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A8R8G8B8toA1R5G5B5(*sB++);
}

void CColorConverter::convert_A8R8G8B8toA1B5G5R5(const void* sP, s32 sN, void* dP) {
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x) {
		s32 r = sB[0] >> 3;
		s32 g = sB[1] >> 3;
		s32 b = sB[2] >> 3;
		s32 a = sB[3] >> 3;

		dB[0] = (a << 15) | (r << 10) | (g << 5) | (b);

		sB += 4;
		dB += 1;
	}
}

void CColorConverter::convert_A8R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP) {
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x) {
		s32 r = sB[2] >> 3;
		s32 g = sB[1] >> 2;
		s32 b = sB[0] >> 3;

		dB[0] = (r << 11) | (g << 5) | (b);

		sB += 4;
		dB += 1;
	}
}

void CColorConverter::convert_R8G8B8toR8G8B8(const void* sP, s32 sN, void* dP) {
	memcpy(dP, sP, sN * 3);
}

void CColorConverter::convert_R8G8B8toA8R8G8B8(const void* sP, s32 sN, void* dP) {
	u8*  sB = (u8* )sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x) {
		*dB = 0xff000000 | (sB[0]<<16) | (sB[1]<<8) | sB[2];

		sB += 3;
		++dB;
	}
}

void CColorConverter::convert_R8G8B8toA1R5G5B5(const void* sP, s32 sN, void* dP) {
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x) {
		s32 r = sB[0] >> 3;
		s32 g = sB[1] >> 3;
		s32 b = sB[2] >> 3;

		dB[0] = (0x8000) | (r << 10) | (g << 5) | (b);

		sB += 3;
		dB += 1;
	}
}

void CColorConverter::convert_A8R8G8B8toA8B8G8R8(const void* sP, s32 sN, void* dP) {
	const u32* sB = (const u32*)sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x) {
		*dB++ = (*sB&0xff00ff00)|((*sB&0x00ff0000)>>16)|((*sB&0x000000ff)<<16);
		++sB;
	}
}

void CColorConverter::convert_R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP) {
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x) {
		s32 r = sB[0] >> 3;
		s32 g = sB[1] >> 2;
		s32 b = sB[2] >> 3;

		dB[0] = (r << 11) | (g << 5) | (b);

		sB += 3;
		dB += 1;
	}
}

void CColorConverter::convert_R5G6B5toR5G6B5(const void* sP, s32 sN, void* dP) {
	memcpy(dP, sP, sN * 2);
}

void CColorConverter::convert_R5G6B5toR8G8B8(const void* sP, s32 sN, void* dP) {
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	for (s32 x = 0; x < sN; ++x) {
		dB[0] = (*sB & 0xf800) >> 8;
		dB[1] = (*sB & 0x07e0) >> 3;
		dB[2] = (*sB & 0x001f) << 3;

		sB += 1;
		dB += 3;
	}
}

void CColorConverter::convert_R5G6B5toA8R8G8B8(const void* sP, s32 sN, void* dP) {
	u16* sB = (u16*)sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = R5G6B5toA8R8G8B8(*sB++);
}

void CColorConverter::convert_R5G6B5toA1R5G5B5(const void* sP, s32 sN, void* dP) {
	u16* sB = (u16*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = R5G6B5toA1R5G5B5(*sB++);
}

bool CColorConverter::canConvertFormat(ECOLOR_FORMAT sourceFormat, ECOLOR_FORMAT destFormat) {
	switch (sourceFormat) {
		case ECF_A1R5G5B5:
			switch (destFormat) {
				case ECF_A1R5G5B5:
				case ECF_R5G6B5:
				case ECF_A8R8G8B8:
				case ECF_R8G8B8:
					return true;
				default:
					break;
			}
		break;
		case ECF_R5G6B5:
			switch (destFormat) {
				case ECF_A1R5G5B5:
				case ECF_R5G6B5:
				case ECF_A8R8G8B8:
				case ECF_R8G8B8:
					return true;
				default:
					break;
			}
		break;
		case ECF_A8R8G8B8:
			switch (destFormat) {
				case ECF_A1R5G5B5:
				case ECF_R5G6B5:
				case ECF_A8R8G8B8:
				case ECF_R8G8B8:
					return true;
				default:
					break;
			}
		break;
		case ECF_R8G8B8:
			switch (destFormat) {
				case ECF_A1R5G5B5:
				case ECF_R5G6B5:
				case ECF_A8R8G8B8:
				case ECF_R8G8B8:
					return true;
				default:
					break;
			}
		break;
		default:
			break;
	}
	return false;
}

void CColorConverter::convert_viaFormat(const void* sP, ECOLOR_FORMAT sF, s32 sN,
				void* dP, ECOLOR_FORMAT dF) {
	// please also update can_convert_viaFormat when adding new conversions
	switch (sF) {
		case ECF_A1R5G5B5:
			switch (dF) {
				case ECF_A1R5G5B5:
					convert_A1R5G5B5toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_A1R5G5B5toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_A1R5G5B5toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_A1R5G5B5toR8G8B8(sP, sN, dP);
				break;
				IRR_CASE_IIMAGE_COMPRESSED_FORMAT
					os::Printer::log("CColorConverter::convert_viaFormat method doesn't support compressed images.", ELL_WARNING);
				break;
#ifndef _DEBUG
				default:
					break;
#endif
			}
		break;
		case ECF_R5G6B5:
			switch (dF) {
				case ECF_A1R5G5B5:
					convert_R5G6B5toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_R5G6B5toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_R5G6B5toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_R5G6B5toR8G8B8(sP, sN, dP);
				break;
				IRR_CASE_IIMAGE_COMPRESSED_FORMAT
					os::Printer::log("CColorConverter::convert_viaFormat method doesn't support compressed images.", ELL_WARNING);
				break;
#ifndef _DEBUG
				default:
					break;
#endif
			}
		break;
		case ECF_A8R8G8B8:
			switch (dF) {
				case ECF_A1R5G5B5:
					convert_A8R8G8B8toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_A8R8G8B8toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_A8R8G8B8toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_A8R8G8B8toR8G8B8(sP, sN, dP);
				break;
				IRR_CASE_IIMAGE_COMPRESSED_FORMAT
					os::Printer::log("CColorConverter::convert_viaFormat method doesn't support compressed images.", ELL_WARNING);
				break;
#ifndef _DEBUG
				default:
					break;
#endif
			}
		break;
		case ECF_R8G8B8:
			switch (dF) {
				case ECF_A1R5G5B5:
					convert_R8G8B8toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_R8G8B8toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_R8G8B8toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_R8G8B8toR8G8B8(sP, sN, dP);
				break;
				IRR_CASE_IIMAGE_COMPRESSED_FORMAT
					os::Printer::log("CColorConverter::convert_viaFormat method doesn't support compressed images.", ELL_WARNING);
				break;
#ifndef _DEBUG
				default:
					break;
#endif
			}
		break;
		IRR_CASE_IIMAGE_COMPRESSED_FORMAT
			os::Printer::log("CColorConverter::convert_viaFormat method doesn't support compressed images.", ELL_WARNING);
			break;
#ifndef _DEBUG
		default:
		break;
#endif
	}
}

} // end namespace video
} // end namespace irr
