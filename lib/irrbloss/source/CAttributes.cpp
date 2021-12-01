// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CAttributes.h"
#include "CAttributeImpl.h"
#include "ITexture.h"
#include "IVideoDriver.h"
#include "os.h"

namespace irr {
namespace io {

CAttributes::CAttributes(video::IVideoDriver* driver)
: Driver(driver) {
	#ifdef _DEBUG
	setDebugName("CAttributes");
	#endif

	if (Driver)
		Driver->grab();
}

CAttributes::~CAttributes() {
	clear();

	if (Driver)
		Driver->drop();
}

//! Removes all attributes
void CAttributes::clear() {
	for (u32 i=0; i<Attributes.size(); ++i)
		Attributes[i]->drop();

	Attributes.clear();
}

//! Sets a string attribute.
//! \param attributeName: Name for the attribute
//! \param value: Value for the attribute. Set this to 0 to delete the attribute
void CAttributes::setAttribute(const c8* attributeName, const c8* value) {
	for (u32 i=0; i<Attributes.size(); ++i)
		if (Attributes[i]->Name == attributeName) {
			if (!value) {
				Attributes[i]->drop();
				Attributes.erase(i);
			} else
				Attributes[i]->setString(value);

			return;
		}

	if (value) {
		Attributes.push_back(new CStringAttribute(attributeName, value));
	}
}

//! Returns attribute index from name, -1 if not found
s32 CAttributes::findAttribute(const c8* attributeName) const {
	for (u32 i=0; i<Attributes.size(); ++i)
		if (Attributes[i]->Name == attributeName)
			return i;

	return -1;
}

IAttribute* CAttributes::getAttributeP(const c8* attributeName) const {
	for (u32 i=0; i<Attributes.size(); ++i)
		if (Attributes[i]->Name == attributeName)
			return Attributes[i];

	return 0;
}

//! Sets a attribute as integer value
void CAttributes::setAttribute(const c8* attributeName, s32 value) {
	IAttribute* att = getAttributeP(attributeName);
	if (att)
		att->setInt(value);
	else {
		Attributes.push_back(new CIntAttribute(attributeName, value));
	}
}

//! Gets a attribute as integer value
//! \param attributeName: Name of the attribute to get.
//! \return Returns value of the attribute previously set by setAttribute() as integer
//! or 0 if attribute is not set.
s32 CAttributes::getAttributeAsInt(const c8* attributeName, irr::s32 defaultNotFound) const {
	const IAttribute* att = getAttributeP(attributeName);
	if (att)
		return att->getInt();
	else
		return defaultNotFound;
}

//! Sets a attribute as float value
void CAttributes::setAttribute(const c8* attributeName, f32 value) {
	IAttribute* att = getAttributeP(attributeName);
	if (att)
		att->setFloat(value);
	else
		Attributes.push_back(new CFloatAttribute(attributeName, value));
}

//! Gets a attribute as integer value
//! \param attributeName: Name of the attribute to get.
//! \return Returns value of the attribute previously set by setAttribute() as float value
//! or 0 if attribute is not set.
f32 CAttributes::getAttributeAsFloat(const c8* attributeName, irr::f32 defaultNotFound) const {
	const IAttribute* att = getAttributeP(attributeName);
	if (att)
		return att->getFloat();

	return defaultNotFound;
}

//! Returns amount of string attributes set in this scene manager.
u32 CAttributes::getAttributeCount() const {
	return Attributes.size();
}

//! Returns string attribute name by index.
//! \param index: Index value, must be between 0 and getStringAttributeCount()-1.
const c8* CAttributes::getAttributeName(s32 index) const {
	if ((u32)index >= Attributes.size())
		return 0;

	return Attributes[index]->Name.c_str();
}

//! Returns the type of an attribute
E_ATTRIBUTE_TYPE CAttributes::getAttributeType(const c8* attributeName) const {
	E_ATTRIBUTE_TYPE ret = EAT_UNKNOWN;

	const IAttribute* att = getAttributeP(attributeName);
	if (att)
		ret = att->getType();

	return ret;
}

//! Returns attribute type by index.
//! \param index: Index value, must be between 0 and getAttributeCount()-1.
E_ATTRIBUTE_TYPE CAttributes::getAttributeType(s32 index) const {
	if ((u32)index >= Attributes.size())
		return EAT_UNKNOWN;

	return Attributes[index]->getType();
}

//! Gets an attribute as integer value
//! \param index: Index value, must be between 0 and getAttributeCount()-1.
s32 CAttributes::getAttributeAsInt(s32 index) const {
	if ((u32)index < Attributes.size())
		return Attributes[index]->getInt();
	else
		return 0;
}

//! Gets an attribute as float value
//! \param index: Index value, must be between 0 and getAttributeCount()-1.
f32 CAttributes::getAttributeAsFloat(s32 index) const {
	if ((u32)index < Attributes.size())
		return Attributes[index]->getFloat();
	else
		return 0.f;
}

//! Adds an attribute as integer
void CAttributes::addInt(const c8* attributeName, s32 value) {
	Attributes.push_back(new CIntAttribute(attributeName, value));
}

//! Adds an attribute as float
void CAttributes::addFloat(const c8* attributeName, f32 value) {
	Attributes.push_back(new CFloatAttribute(attributeName, value));
}

//! Adds an attribute as string
void CAttributes::addString(const c8* attributeName, const char* value) {
	Attributes.push_back(new CStringAttribute(attributeName, value));
}

//! Returns if an attribute with a name exists
bool CAttributes::existsAttribute(const c8* attributeName) const {
	return getAttributeP(attributeName) != 0;
}

//! Sets an attribute value as string.
//! \param attributeName: Name for the attribute
void CAttributes::setAttribute(s32 index, const c8* value) {
	if ((u32)index < Attributes.size())
		Attributes[index]->setString(value);
}

//! Sets an attribute as integer value
void CAttributes::setAttribute(s32 index, s32 value) {
	if ((u32)index < Attributes.size())
		Attributes[index]->setInt(value);
}

//! Sets a attribute as float value
void CAttributes::setAttribute(s32 index, f32 value) {
	if ((u32)index < Attributes.size())
		Attributes[index]->setFloat(value);
}

} // end namespace io
} // end namespace irr

