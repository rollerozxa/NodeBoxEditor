// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_MENU_H_INCLUDED__
#define __C_GUI_MENU_H_INCLUDED__

#include "IrrCompileConfig.h"
#include "CGUIContextMenu.h"

namespace irr {
namespace gui
{

	//! GUI menu interface.
	class CGUIMenu : public CGUIContextMenu
	{
	public:

		//! constructor
		CGUIMenu(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! draws the element and its children
		virtual void draw() _IRR_OVERRIDE_;

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_;

		//! Updates the absolute position.
		virtual void updateAbsolutePosition() _IRR_OVERRIDE_;

	protected:

		virtual void recalculateSize() _IRR_OVERRIDE_;

		//! returns the item highlight-area
		virtual core::rect<s32> getHRect(const SItem& i, const core::rect<s32>& absolute) const _IRR_OVERRIDE_;

		//! Gets drawing rect of Item
		virtual core::rect<s32> getRect(const SItem& i, const core::rect<s32>& absolute) const _IRR_OVERRIDE_;
	};

} // end namespace gui
} // end namespace irr


#endif //  __C_GUI_MENU_H_INCLUDED__
