// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CAttributes.h"
#include "fast_atof.h"
#include "ITexture.h"
#include "IVideoDriver.h"

namespace irr {
namespace io {

/*
	Basic types, check documentation in IAttribute.h to see how they generally work.
*/

// Attribute implemented for integers
class CIntAttribute : public IAttribute {
public:

	CIntAttribute(const char* name, s32 value) {
		Name = name;
		setInt(value);
	}

	virtual s32 getInt() const _IRR_OVERRIDE_
	{
		return Value;
	}

	virtual f32 getFloat() const _IRR_OVERRIDE_
	{
		return (f32)Value;
	}

	virtual bool getBool() const _IRR_OVERRIDE_
	{
		return (Value != 0);
	}

	virtual core::stringw getStringW() const _IRR_OVERRIDE_
	{
		return core::stringw(Value);
	}

	virtual void setInt(s32 intValue) _IRR_OVERRIDE_
	{
		Value = intValue;
	}

	virtual void setFloat(f32 floatValue) _IRR_OVERRIDE_
	{
		Value = (s32)floatValue;
	};

	virtual void setString(const char* text) _IRR_OVERRIDE_
	{
		Value = atoi(text);
	}

	virtual E_ATTRIBUTE_TYPE getType() const _IRR_OVERRIDE_
	{
		return EAT_INT;
	}


	virtual const wchar_t* getTypeString() const _IRR_OVERRIDE_
	{
		return L"int";
	}

	s32 Value;
};

// Attribute implemented for floats
class CFloatAttribute : public IAttribute {
public:

	CFloatAttribute(const char* name, f32 value) {
		Name = name;
		setFloat(value);
	}

	virtual s32 getInt() const _IRR_OVERRIDE_
	{
		return (s32)Value;
	}

	virtual f32 getFloat() const _IRR_OVERRIDE_
	{
		return Value;
	}

	virtual bool getBool() const _IRR_OVERRIDE_
	{
		return (Value != 0);
	}

	virtual core::stringw getStringW() const _IRR_OVERRIDE_
	{
		return core::stringw((double)Value);
	}

	virtual void setInt(s32 intValue) _IRR_OVERRIDE_
	{
		Value = (f32)intValue;
	}

	virtual void setFloat(f32 floatValue) _IRR_OVERRIDE_
	{
		Value = floatValue;
	}

	virtual void setString(const char* text) _IRR_OVERRIDE_
	{
		Value = core::fast_atof(text);
	}

	virtual E_ATTRIBUTE_TYPE getType() const _IRR_OVERRIDE_
	{
		return EAT_FLOAT;
	}


	virtual const wchar_t* getTypeString() const _IRR_OVERRIDE_
	{
		return L"float";
	}

	f32 Value;
};

/*
	Special attributes
*/


// Attribute implemented for strings
class CStringAttribute : public IAttribute {
public:

	CStringAttribute(const char* name, const char* value) {
		IsStringW=false;
		Name = name;
		setString(value);
	}

	CStringAttribute(const char* name, const wchar_t* value) {
		IsStringW = true;
		Name = name;
		setString(value);
	}

	CStringAttribute(const char* name, void* binaryData, s32 lengthInBytes) {
		IsStringW=false;
		Name = name;
		setBinary(binaryData, lengthInBytes);
	}

	virtual s32 getInt() const _IRR_OVERRIDE_
	{
		if (IsStringW)
			return atoi(core::stringc(ValueW.c_str()).c_str());
		else
			return atoi(Value.c_str());
	}

	virtual f32 getFloat() const _IRR_OVERRIDE_
	{
		if (IsStringW)
			return core::fast_atof(core::stringc(ValueW.c_str()).c_str());
		else
			return core::fast_atof(Value.c_str());
	}

	virtual bool getBool() const _IRR_OVERRIDE_
	{
		if (IsStringW)
			return ValueW.equals_ignore_case(L"true");
		else
			return Value.equals_ignore_case("true");
	}

	virtual core::stringc getString() const _IRR_OVERRIDE_
	{
		if (IsStringW)
			return core::stringc(ValueW.c_str());
		else
			return Value;
	}
	virtual core::stringw getStringW() const _IRR_OVERRIDE_
	{
		if (IsStringW)
			return ValueW;
		else
			return core::stringw(Value.c_str());
	}

	virtual void setInt(s32 intValue) _IRR_OVERRIDE_
	{
		if (IsStringW)
			ValueW = core::stringw(intValue);
		else
			Value = core::stringc(intValue);
	}

	virtual void setFloat(f32 floatValue) _IRR_OVERRIDE_
	{
		if (IsStringW) {
			ValueW = core::stringw((double)floatValue);
		} else {
			Value = core::stringc((double)floatValue);
		}
	};

	virtual void setString(const char* text) _IRR_OVERRIDE_
	{
		if (IsStringW)
			ValueW = core::stringw(text);
		else
			Value = text;
	}

	virtual void setString(const wchar_t* text) _IRR_OVERRIDE_
	{
		if (IsStringW)
			ValueW = text;
		else
			Value = core::stringc(text);
	}

	virtual E_ATTRIBUTE_TYPE getType() const _IRR_OVERRIDE_
	{
		return EAT_STRING;
	}


	virtual const wchar_t* getTypeString() const _IRR_OVERRIDE_
	{
		return L"string";
	}

	virtual void getBinary(void* outdata, s32 maxLength) const _IRR_OVERRIDE_
	{
		const s32 dataSize = maxLength;
		c8* datac8 = (c8*)(outdata);
		s32 p = 0;
		const c8* dataString = Value.c_str();

		for (s32 i=0; i<dataSize; ++i)
			datac8[i] = 0;

		while(dataString[p] && p<dataSize) {
			s32 v = getByteFromHex((c8)dataString[p*2]) * 16;

			if (dataString[(p*2)+1])
				v += getByteFromHex((c8)dataString[(p*2)+1]);

			datac8[p] = v;
			++p;
		}
	};

	virtual void setBinary(void* data, s32 maxLength) _IRR_OVERRIDE_
	{
		const s32 dataSize = maxLength;
		const c8* datac8 = (c8*)(data);
		char tmp[3];
		tmp[2] = 0;
		Value = "";

		for (s32 b=0; b<dataSize; ++b) {
			getHexStrFromByte(datac8[b], tmp);
			Value.append(tmp);
		}
	};

	bool IsStringW;
	core::stringc Value;
	core::stringw ValueW;

protected:

	static inline s32 getByteFromHex(c8 h) {
		if (h >= '0' && h <='9')
			return h-'0';

		if (h >= 'a' && h <='f')
			return h-'a' + 10;

		return 0;
	}

	static inline void getHexStrFromByte(c8 byte, c8* out) {
		s32 b = (byte & 0xf0) >> 4;

		for (s32 i=0; i<2; ++i) {
			if (b >=0 && b <= 9)
				out[i] = b+'0';
			if (b >=10 && b <= 15)
				out[i] = (b-10)+'a';

			b = byte & 0x0f;
		}
	}
};

// todo: CGUIFontAttribute

} // end namespace io
} // end namespace irr
