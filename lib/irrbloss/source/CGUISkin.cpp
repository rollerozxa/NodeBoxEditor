// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUISkin.h"
#include "IGUIFont.h"
#include "IGUISpriteBank.h"
#include "IGUIElement.h"
#include "IVideoDriver.h"

namespace irr {
namespace gui
{

CGUISkin::CGUISkin(EGUI_SKIN_TYPE type, video::IVideoDriver* driver)
: SpriteBank(0), Driver(driver), Type(type) {
	#ifdef _DEBUG
	setDebugName("CGUISkin");
	#endif

	Colors[EGDC_3D_DARK_SHADOW]     = video::SColor(255,50,50,50);
	Colors[EGDC_3D_SHADOW]          = video::SColor(255,130,130,130);
	Colors[EGDC_3D_FACE]            = video::SColor(255,190,190,190);
	Colors[EGDC_3D_HIGH_LIGHT]      = video::SColor(255,255,255,255);
	Colors[EGDC_3D_LIGHT]           = video::SColor(255,210,210,210);
	Colors[EGDC_ACTIVE_BORDER]      = video::SColor(255,0,107,25);
	Colors[EGDC_ACTIVE_CAPTION]     = video::SColor(255,255,255,255);
	Colors[EGDC_APP_WORKSPACE]      = video::SColor(101,100,100,100);
	Colors[EGDC_BUTTON_TEXT]        = video::SColor(240,10,10,10);
	Colors[EGDC_GRAY_TEXT]          = video::SColor(240,130,130,130);
	Colors[EGDC_HIGH_LIGHT]         = video::SColor(255,0,107,25);
	Colors[EGDC_HIGH_LIGHT_TEXT]    = video::SColor(240,255,255,255);
	Colors[EGDC_INACTIVE_BORDER]    = video::SColor(101,165,165,165);
	Colors[EGDC_INACTIVE_CAPTION]   = video::SColor(255,30,30,30);
	Colors[EGDC_TOOLTIP]            = video::SColor(200,0,0,0);
	Colors[EGDC_TOOLTIP_BACKGROUND] = video::SColor(200,255,255,225);
	Colors[EGDC_SCROLLBAR]          = video::SColor(101,230,230,230);
	Colors[EGDC_WINDOW]             = video::SColor(101,255,255,255);
	Colors[EGDC_WINDOW_SYMBOL]      = video::SColor(200,10,10,10);
	Colors[EGDC_ICON]               = video::SColor(200,255,255,255);
	Colors[EGDC_ICON_HIGH_LIGHT]    = video::SColor(200,8,107,36);
	Colors[EGDC_GRAY_WINDOW_SYMBOL] = video::SColor(240,100,100,100);
	Colors[EGDC_EDITABLE] 			= video::SColor(255,255,255,255);
	Colors[EGDC_GRAY_EDITABLE]		= video::SColor(255,120,120,120);
	Colors[EGDC_FOCUSED_EDITABLE]	= video::SColor(255,240,240,255);

	Icons[EGDI_WINDOW_MAXIMIZE] = 225;
	Icons[EGDI_WINDOW_RESTORE] = 226;
	Icons[EGDI_WINDOW_CLOSE] = 227;
	Icons[EGDI_WINDOW_MINIMIZE] = 228;
	Icons[EGDI_CURSOR_UP] = 229;
	Icons[EGDI_CURSOR_DOWN] = 230;
	Icons[EGDI_CURSOR_LEFT] = 231;
	Icons[EGDI_CURSOR_RIGHT] = 232;
	Icons[EGDI_MENU_MORE] = 232;
	Icons[EGDI_CHECK_BOX_CHECKED] = 233;
	Icons[EGDI_DROP_DOWN] = 234;
	Icons[EGDI_SMALL_CURSOR_UP] = 235;
	Icons[EGDI_SMALL_CURSOR_DOWN] = 236;
	Icons[EGDI_RADIO_BUTTON_CHECKED] = 237;
	Icons[EGDI_MORE_LEFT] = 238;
	Icons[EGDI_MORE_RIGHT] = 239;
	Icons[EGDI_MORE_UP] = 240;
	Icons[EGDI_MORE_DOWN] = 241;
	Icons[EGDI_WINDOW_RESIZE] = 242;
	Icons[EGDI_EXPAND] = 243;
	Icons[EGDI_COLLAPSE] = 244;

	Icons[EGDI_FILE] = 245;
	Icons[EGDI_DIRECTORY] = 246;

	for (u32 i=0; i<EGDF_COUNT; ++i)
		Fonts[i] = 0;
}

//! destructor
CGUISkin::~CGUISkin() {
	for (u32 i=0; i<EGDF_COUNT; ++i) {
		if (Fonts[i])
			Fonts[i]->drop();
	}

	if (SpriteBank)
		SpriteBank->drop();
}

//! returns default color
video::SColor CGUISkin::getColor(EGUI_DEFAULT_COLOR color) const {
	if ((u32)color < EGDC_COUNT)
		return Colors[color];
	else
		return video::SColor();
}

//! sets a default color
void CGUISkin::setColor(EGUI_DEFAULT_COLOR which, video::SColor newColor) {
	if ((u32)which < EGDC_COUNT)
		Colors[which] = newColor;
}

//! returns the default font
IGUIFont* CGUISkin::getFont(EGUI_DEFAULT_FONT which) const {
	if (((u32)which < EGDF_COUNT) && Fonts[which])
		return Fonts[which];
	else
		return Fonts[EGDF_DEFAULT];
}

//! sets a default font
void CGUISkin::setFont(IGUIFont* font, EGUI_DEFAULT_FONT which) {
	if ((u32)which >= EGDF_COUNT)
		return;

	if (font) {
		font->grab();
		if (Fonts[which])
			Fonts[which]->drop();

		Fonts[which] = font;
	}
}

//! gets the sprite bank stored
IGUISpriteBank* CGUISkin::getSpriteBank() const {
	return SpriteBank;
}

//! set a new sprite bank or remove one by passing 0
void CGUISkin::setSpriteBank(IGUISpriteBank* bank) {
	if (bank)
		bank->grab();

	if (SpriteBank)
		SpriteBank->drop();

	SpriteBank = bank;
}

//! draws a standard 3d button pane
/**	Used for drawing for example buttons in normal state.
It uses the colors EGDC_3D_DARK_SHADOW, EGDC_3D_HIGH_LIGHT, EGDC_3D_SHADOW and
EGDC_3D_FACE for this. See EGUI_DEFAULT_COLOR for details.
\param rect: Defining area where to draw.
\param clip: Clip area.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly. */
void CGUISkin::draw3DButtonPaneStandard(IGUIElement* element,
					const core::rect<s32>& r,
					const core::rect<s32>* clip) {
	if (!Driver)
		return;

	core::rect<s32> rect = r;

	Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

	rect.LowerRightCorner.X -= 1;
	rect.LowerRightCorner.Y -= 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);

	rect.UpperLeftCorner.X += 1;
	rect.UpperLeftCorner.Y += 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);

	rect.LowerRightCorner.X -= 1;
	rect.LowerRightCorner.Y -= 1;

	Driver->draw2DRectangle(getColor(EGDC_3D_FACE), rect, clip);
}

//! draws a pressed 3d button pane
/**	Used for drawing for example buttons in pressed state.
It uses the colors EGDC_3D_DARK_SHADOW, EGDC_3D_HIGH_LIGHT, EGDC_3D_SHADOW and
EGDC_3D_FACE for this. See EGUI_DEFAULT_COLOR for details.
\param rect: Defining area where to draw.
\param clip: Clip area.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly. */
void CGUISkin::draw3DButtonPanePressed(IGUIElement* element,
					const core::rect<s32>& r,
					const core::rect<s32>* clip) {
	if (!Driver)
		return;

	core::rect<s32> rect = r;
	Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);

	rect.LowerRightCorner.X -= 1;
	rect.LowerRightCorner.Y -= 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

	rect.UpperLeftCorner.X += 1;
	rect.UpperLeftCorner.Y += 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);

	rect.UpperLeftCorner.X += 1;
	rect.UpperLeftCorner.Y += 1;

	Driver->draw2DRectangle(getColor(EGDC_3D_FACE), rect, clip);
}

//! draws a sunken 3d pane
/** Used for drawing the background of edit, combo or check boxes.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly.
\param bgcolor: Background color.
\param flat: Specifies if the sunken pane should be flat or displayed as sunken
deep into the ground.
\param rect: Defining area where to draw.
\param clip: Clip area.	*/
void CGUISkin::draw3DSunkenPane(IGUIElement* element, video::SColor bgcolor,
				bool flat, bool fillBackGround,
				const core::rect<s32>& r,
				const core::rect<s32>* clip) {
	if (!Driver)
		return;

	core::rect<s32> rect = r;

	if (fillBackGround)
		Driver->draw2DRectangle(bgcolor, rect, clip);

	if (flat) {
		// draw flat sunken pane

		rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);	// top

		++rect.UpperLeftCorner.Y;
		rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
		rect.LowerRightCorner.X = rect.UpperLeftCorner.X + 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);	// left

		rect = r;
		++rect.UpperLeftCorner.Y;
		rect.UpperLeftCorner.X = rect.LowerRightCorner.X - 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);	// right

		rect = r;
		++rect.UpperLeftCorner.X;
		rect.UpperLeftCorner.Y = r.LowerRightCorner.Y - 1;
		--rect.LowerRightCorner.X;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);	// bottom
	} else {
		// draw deep sunken pane
		rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);	// top
		++rect.UpperLeftCorner.X;
		++rect.UpperLeftCorner.Y;
		--rect.LowerRightCorner.X;
		++rect.LowerRightCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

		rect.UpperLeftCorner.X = r.UpperLeftCorner.X;
		rect.UpperLeftCorner.Y = r.UpperLeftCorner.Y+1;
		rect.LowerRightCorner.X = rect.UpperLeftCorner.X + 1;
		rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);	// left
		++rect.UpperLeftCorner.X;
		++rect.UpperLeftCorner.Y;
		++rect.LowerRightCorner.X;
		--rect.LowerRightCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

		rect = r;
		rect.UpperLeftCorner.X = rect.LowerRightCorner.X - 1;
		++rect.UpperLeftCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);	// right
		--rect.UpperLeftCorner.X;
		++rect.UpperLeftCorner.Y;
		--rect.LowerRightCorner.X;
		--rect.LowerRightCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_LIGHT), rect, clip);

		rect = r;
		++rect.UpperLeftCorner.X;
		rect.UpperLeftCorner.Y = r.LowerRightCorner.Y - 1;
		--rect.LowerRightCorner.X;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);	// bottom
		++rect.UpperLeftCorner.X;
		--rect.UpperLeftCorner.Y;
		--rect.LowerRightCorner.X;
		--rect.LowerRightCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_LIGHT), rect, clip);
	}
}

//! draws a window background
// return where to draw title bar text.
core::rect<s32> CGUISkin::draw3DWindowBackground(IGUIElement* element,
				bool drawTitleBar, video::SColor titleBarColor,
				const core::rect<s32>& r,
				const core::rect<s32>* clip,
				core::rect<s32>* checkClientArea) {
	if (!Driver) {
		if ( checkClientArea ) {
			*checkClientArea = r;
		}
		return r;
	}

	core::rect<s32> rect = r;

	// top border
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 1;
	if ( !checkClientArea ) {
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);
	}

	// left border
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	rect.LowerRightCorner.X = rect.UpperLeftCorner.X + 1;
	if ( !checkClientArea ) {
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);
	}

	// right border dark outer line
	rect.UpperLeftCorner.X = r.LowerRightCorner.X - 1;
	rect.LowerRightCorner.X = r.LowerRightCorner.X;
	rect.UpperLeftCorner.Y = r.UpperLeftCorner.Y;
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	if ( !checkClientArea ) {
		Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);
	}

	// right border bright innner line
	rect.UpperLeftCorner.X -= 1;
	rect.LowerRightCorner.X -= 1;
	rect.UpperLeftCorner.Y += 1;
	rect.LowerRightCorner.Y -= 1;
	if ( !checkClientArea ) {
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);
	}

	// bottom border dark outer line
	rect.UpperLeftCorner.X = r.UpperLeftCorner.X;
	rect.UpperLeftCorner.Y = r.LowerRightCorner.Y - 1;
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	rect.LowerRightCorner.X = r.LowerRightCorner.X;
	if ( !checkClientArea ) {
		Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);
	}

	// bottom border bright inner line
	rect.UpperLeftCorner.X += 1;
	rect.LowerRightCorner.X -= 1;
	rect.UpperLeftCorner.Y -= 1;
	rect.LowerRightCorner.Y -= 1;
	if ( !checkClientArea ) {
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);
	}

	// client area for background
	rect = r;
	rect.UpperLeftCorner.X +=1;
	rect.UpperLeftCorner.Y +=1;
	rect.LowerRightCorner.X -= 2;
	rect.LowerRightCorner.Y -= 2;
	if (checkClientArea) {
		*checkClientArea = rect;
	}

	if ( !checkClientArea ) {
		Driver->draw2DRectangle(getColor(EGDC_3D_FACE), rect, clip);
	}

	// title bar
	rect = r;
	rect.UpperLeftCorner.X += 2;
	rect.UpperLeftCorner.Y += 2;
	rect.LowerRightCorner.X -= 2;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 17;

	if (drawTitleBar ) {
		if (checkClientArea) {
			(*checkClientArea).UpperLeftCorner.Y = rect.LowerRightCorner.Y;
		} else {
			Driver->draw2DRectangle(titleBarColor, rect, clip);
		}
	}

	return rect;
}

//! draws a standard 3d menu pane
/**	Used for drawing for menus and context menus.
It uses the colors EGDC_3D_DARK_SHADOW, EGDC_3D_HIGH_LIGHT, EGDC_3D_SHADOW and
EGDC_3D_FACE for this. See EGUI_DEFAULT_COLOR for details.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly.
\param rect: Defining area where to draw.
\param clip: Clip area.	*/
void CGUISkin::draw3DMenuPane(IGUIElement* element,
			const core::rect<s32>& r, const core::rect<s32>* clip) {
	if (!Driver)
		return;

	core::rect<s32> rect = r;

	rect.UpperLeftCorner.Y -= 3;
	draw3DButtonPaneStandard(element, rect, clip);
	return;

	// in this skin, this is exactly what non pressed buttons look like,
	// so we could simply call
	// draw3DButtonPaneStandard(element, rect, clip);
	// here.
	// but if the skin is transparent, this doesn't look that nice. So
	// We draw it a little bit better, with some more draw2DRectangle calls,
	// but there aren't that much menus visible anyway.

	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);

	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	rect.LowerRightCorner.X = rect.UpperLeftCorner.X + 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);

	rect.UpperLeftCorner.X = r.LowerRightCorner.X - 1;
	rect.LowerRightCorner.X = r.LowerRightCorner.X;
	rect.UpperLeftCorner.Y = r.UpperLeftCorner.Y;
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

	rect.UpperLeftCorner.X -= 1;
	rect.LowerRightCorner.X -= 1;
	rect.UpperLeftCorner.Y += 1;
	rect.LowerRightCorner.Y -= 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);

	rect.UpperLeftCorner.X = r.UpperLeftCorner.X;
	rect.UpperLeftCorner.Y = r.LowerRightCorner.Y - 1;
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	rect.LowerRightCorner.X = r.LowerRightCorner.X;
	Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

	rect.UpperLeftCorner.X += 1;
	rect.LowerRightCorner.X -= 1;
	rect.UpperLeftCorner.Y -= 1;
	rect.LowerRightCorner.Y -= 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);

	rect = r;
	rect.UpperLeftCorner.X +=1;
	rect.UpperLeftCorner.Y +=1;
	rect.LowerRightCorner.X -= 2;
	rect.LowerRightCorner.Y -= 2;

	Driver->draw2DRectangle(getColor(EGDC_3D_FACE), rect, clip);
}

//! draws a standard 3d tool bar
/**	Used for drawing for toolbars and menus.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly.
\param rect: Defining area where to draw.
\param clip: Clip area.	*/
void CGUISkin::draw3DToolBar(IGUIElement* element,
				const core::rect<s32>& r,
				const core::rect<s32>* clip) {
	if (!Driver)
		return;

	core::rect<s32> rect = r;

	rect.UpperLeftCorner.X = r.UpperLeftCorner.X;
	rect.UpperLeftCorner.Y = r.LowerRightCorner.Y - 1;
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	rect.LowerRightCorner.X = r.LowerRightCorner.X;
	Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);

	rect = r;
	rect.LowerRightCorner.Y -= 1;

	Driver->draw2DRectangle(getColor(EGDC_3D_FACE), rect, clip);
}

//! draws an icon, usually from the skin's sprite bank
/**	\param parent: Pointer to the element which wishes to draw this icon.
This parameter is usually not used by IGUISkin, but can be used for example
by more complex implementations to find out how to draw the part exactly.
\param icon: Specifies the icon to be drawn.
\param position: The position to draw the icon
\param starttime: The time at the start of the animation
\param currenttime: The present time, used to calculate the frame number
\param loop: Whether the animation should loop or not
\param clip: Clip area.	*/
void CGUISkin::drawIcon(IGUIElement* element, EGUI_DEFAULT_ICON icon,
			const core::position2di position,
			u32 starttime, u32 currenttime,
			bool loop, const core::rect<s32>* clip) {
	if (!SpriteBank)
		return;

	bool gray = element && !element->isEnabled();
	SpriteBank->draw2DSprite(Icons[icon], position, clip,
			Colors[gray? EGDC_GRAY_WINDOW_SYMBOL : EGDC_WINDOW_SYMBOL], starttime, currenttime, loop, true);
}

EGUI_SKIN_TYPE CGUISkin::getType() const {
	return Type;
}

//! draws a 2d rectangle.
void CGUISkin::draw2DRectangle(IGUIElement* element,
		const video::SColor &color, const core::rect<s32>& pos,
		const core::rect<s32>* clip) {
	Driver->draw2DRectangle(color, pos, clip);
}

} // end namespace gui
} // end namespace irr
