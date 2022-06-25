// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_

#if defined (__STRICT_ANSI__)
	#error Compiling with __STRICT_ANSI__ not supported. g++ does set this when compiling with -std=c++11 or -std=c++0x. Use instead -std=gnu++11 or -std=gnu++0x. Or use -U__STRICT_ANSI__ to disable strict ansi.
#endif

#include "CIrrDeviceWin32.h"
#include "IEventReceiver.h"
#include "irrList.h"
#include "os.h"

#include "CTimer.h"
#include "irrString.h"
#include "COSOperator.h"
#include "dimension2d.h"
#include "IGUISpriteBank.h"
#include <winuser.h>
#include "SExposedVideoData.h"

#if defined(_IRR_COMPILE_WITH_OPENGL_)
#include "CWGLManager.h"
#endif

namespace irr {
	namespace video
	{
#ifdef _IRR_COMPILE_WITH_OPENGL_
		IVideoDriver* createOpenGLDriver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);
#endif
	}
} // end namespace irr

// Get the codepage from the locale language id
// Based on the table from http://www.science.co.il/Language/Locale-Codes.asp?s=decimal
static unsigned int LocaleIdToCodepage(unsigned int lcid) {
	switch ( lcid ) {
		case 1098:  // Telugu
		case 1095:  // Gujarati
		case 1094:  // Punjabi
		case 1103:  // Sanskrit
		case 1111:  // Konkani
		case 1114:  // Syriac
		case 1099:  // Kannada
		case 1102:  // Marathi
		case 1125:  // Divehi
		case 1067:  // Armenian
		case 1081:  // Hindi
		case 1079:  // Georgian
		case 1097:  // Tamil
			return 0;
		case 1054:  // Thai
			return 874;
		case 1041:  // Japanese
			return 932;
		case 2052:  // Chinese (PRC)
		case 4100:  // Chinese (Singapore)
			return 936;
		case 1042:  // Korean
			return 949;
		case 5124:  // Chinese (Macau S.A.R.)
		case 3076:  // Chinese (Hong Kong S.A.R.)
		case 1028:  // Chinese (Taiwan)
			return 950;
		case 1048:  // Romanian
		case 1060:  // Slovenian
		case 1038:  // Hungarian
		case 1051:  // Slovak
		case 1045:  // Polish
		case 1052:  // Albanian
		case 2074:  // Serbian (Latin)
		case 1050:  // Croatian
		case 1029:  // Czech
			return 1250;
		case 1104:  // Mongolian (Cyrillic)
		case 1071:  // FYRO Macedonian
		case 2115:  // Uzbek (Cyrillic)
		case 1058:  // Ukrainian
		case 2092:  // Azeri (Cyrillic)
		case 1092:  // Tatar
		case 1087:  // Kazakh
		case 1059:  // Belarusian
		case 1088:  // Kyrgyz (Cyrillic)
		case 1026:  // Bulgarian
		case 3098:  // Serbian (Cyrillic)
		case 1049:  // Russian
			return 1251;
		case 8201:  // English (Jamaica)
		case 3084:  // French (Canada)
		case 1036:  // French (France)
		case 5132:  // French (Luxembourg)
		case 5129:  // English (New Zealand)
		case 6153:  // English (Ireland)
		case 1043:  // Dutch (Netherlands)
		case 9225:  // English (Caribbean)
		case 4108:  // French (Switzerland)
		case 4105:  // English (Canada)
		case 1110:  // Galician
		case 10249:  // English (Belize)
		case 3079:  // German (Austria)
		case 6156:  // French (Monaco)
		case 12297:  // English (Zimbabwe)
		case 1069:  // Basque
		case 2067:  // Dutch (Belgium)
		case 2060:  // French (Belgium)
		case 1035:  // Finnish
		case 1080:  // Faroese
		case 1031:  // German (Germany)
		case 3081:  // English (Australia)
		case 1033:  // English (United States)
		case 2057:  // English (United Kingdom)
		case 1027:  // Catalan
		case 11273:  // English (Trinidad)
		case 7177:  // English (South Africa)
		case 1030:  // Danish
		case 13321:  // English (Philippines)
		case 15370:  // Spanish (Paraguay)
		case 9226:  // Spanish (Colombia)
		case 5130:  // Spanish (Costa Rica)
		case 7178:  // Spanish (Dominican Republic)
		case 12298:  // Spanish (Ecuador)
		case 17418:  // Spanish (El Salvador)
		case 4106:  // Spanish (Guatemala)
		case 18442:  // Spanish (Honduras)
		case 3082:  // Spanish (International Sort)
		case 13322:  // Spanish (Chile)
		case 19466:  // Spanish (Nicaragua)
		case 2058:  // Spanish (Mexico)
		case 10250:  // Spanish (Peru)
		case 20490:  // Spanish (Puerto Rico)
		case 1034:  // Spanish (Traditional Sort)
		case 14346:  // Spanish (Uruguay)
		case 8202:  // Spanish (Venezuela)
		case 1089:  // Swahili
		case 1053:  // Swedish
		case 2077:  // Swedish (Finland)
		case 5127:  // German (Liechtenstein)
		case 1078:  // Afrikaans
		case 6154:  // Spanish (Panama)
		case 4103:  // German (Luxembourg)
		case 16394:  // Spanish (Bolivia)
		case 2055:  // German (Switzerland)
		case 1039:  // Icelandic
		case 1057:  // Indonesian
		case 1040:  // Italian (Italy)
		case 2064:  // Italian (Switzerland)
		case 2068:  // Norwegian (Nynorsk)
		case 11274:  // Spanish (Argentina)
		case 1046:  // Portuguese (Brazil)
		case 1044:  // Norwegian (Bokmal)
		case 1086:  // Malay (Malaysia)
		case 2110:  // Malay (Brunei Darussalam)
		case 2070:  // Portuguese (Portugal)
			return 1252;
		case 1032:  // Greek
			return 1253;
		case 1091:  // Uzbek (Latin)
		case 1068:  // Azeri (Latin)
		case 1055:  // Turkish
			return 1254;
		case 1037:  // Hebrew
			return 1255;
		case 5121:  // Arabic (Algeria)
		case 15361:  // Arabic (Bahrain)
		case 9217:  // Arabic (Yemen)
		case 3073:  // Arabic (Egypt)
		case 2049:  // Arabic (Iraq)
		case 11265:  // Arabic (Jordan)
		case 13313:  // Arabic (Kuwait)
		case 12289:  // Arabic (Lebanon)
		case 4097:  // Arabic (Libya)
		case 6145:  // Arabic (Morocco)
		case 8193:  // Arabic (Oman)
		case 16385:  // Arabic (Qatar)
		case 1025:  // Arabic (Saudi Arabia)
		case 10241:  // Arabic (Syria)
		case 14337:  // Arabic (U.A.E.)
		case 1065:  // Farsi
		case 1056:  // Urdu
		case 7169:  // Arabic (Tunisia)
			return 1256;
		case 1061:  // Estonian
		case 1062:  // Latvian
		case 1063:  // Lithuanian
			return 1257;
		case 1066:  // Vietnamese
			return 1258;
	}
	return 65001;   // utf-8
}

namespace {
	struct SEnvMapper
	{
		HWND hWnd;
		irr::CIrrDeviceWin32* irrDev;
	};
	// NOTE: This is global. We can have more than one Irrlicht Device at same time.
	irr::core::array<SEnvMapper> EnvMap;

	HKL KEYBOARD_INPUT_HKL=0;
	unsigned int KEYBOARD_INPUT_CODEPAGE = 1252;
}

irr::CIrrDeviceWin32* getDeviceFromHWnd(HWND hWnd) {
	const irr::u32 end = EnvMap.size();
	for ( irr::u32 i=0; i < end; ++i ) {
		const SEnvMapper& env = EnvMap[i];
		if ( env.hWnd == hWnd )
			return env.irrDev;
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL 0x020A
	#endif
	#ifndef WHEEL_DELTA
	#define WHEEL_DELTA 120
	#endif

	irr::CIrrDeviceWin32* dev = 0;
	irr::SEvent event;

	static irr::s32 ClickCount=0;
	if (GetCapture() != hWnd && ClickCount > 0)
		ClickCount = 0;

	struct messageMap
	{
		irr::s32 group;
		UINT winMessage;
		irr::s32 irrMessage;
	};

	static messageMap mouseMap[] =
	{
		{0, WM_LBUTTONDOWN, irr::EMIE_LMOUSE_PRESSED_DOWN},
		{1, WM_LBUTTONUP,   irr::EMIE_LMOUSE_LEFT_UP},
		{0, WM_RBUTTONDOWN, irr::EMIE_RMOUSE_PRESSED_DOWN},
		{1, WM_RBUTTONUP,   irr::EMIE_RMOUSE_LEFT_UP},
		{0, WM_MBUTTONDOWN, irr::EMIE_MMOUSE_PRESSED_DOWN},
		{1, WM_MBUTTONUP,   irr::EMIE_MMOUSE_LEFT_UP},
		{2, WM_MOUSEMOVE,   irr::EMIE_MOUSE_MOVED},
		{3, WM_MOUSEWHEEL,  irr::EMIE_MOUSE_WHEEL},
		{-1, 0, 0}
	};

	// handle grouped events
	messageMap * m = mouseMap;
	while ( m->group >=0 && m->winMessage != message )
		m += 1;

	if ( m->group >= 0 ) {
		if ( m->group == 0 )	// down
		{
			ClickCount++;
			SetCapture(hWnd);
		}
		else
		if ( m->group == 1 )	// up
		{
			ClickCount--;
			if (ClickCount<1) {
				ClickCount=0;
				ReleaseCapture();
			}
		}

		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = (irr::EMOUSE_INPUT_EVENT) m->irrMessage;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		event.MouseInput.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
		event.MouseInput.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
		// left and right mouse buttons
		event.MouseInput.ButtonStates = wParam & ( MK_LBUTTON | MK_RBUTTON);
		// middle and extra buttons
		if (wParam & MK_MBUTTON)
			event.MouseInput.ButtonStates |= irr::EMBSM_MIDDLE;
#if(_WIN32_WINNT >= 0x0500)
		if (wParam & MK_XBUTTON1)
			event.MouseInput.ButtonStates |= irr::EMBSM_EXTRA1;
		if (wParam & MK_XBUTTON2)
			event.MouseInput.ButtonStates |= irr::EMBSM_EXTRA2;
#endif
		event.MouseInput.Wheel = 0.f;

		// wheel
		if ( m->group == 3 ) {
			POINT p; // fixed by jox
			p.x = 0; p.y = 0;
			ClientToScreen(hWnd, &p);
			event.MouseInput.X -= p.x;
			event.MouseInput.Y -= p.y;
			event.MouseInput.Wheel = ((irr::f32)((short)HIWORD(wParam))) / (irr::f32)WHEEL_DELTA;
		}

		dev = getDeviceFromHWnd(hWnd);
		if (dev) {
			dev->postEventFromUser(event);

			if ( event.MouseInput.Event >= irr::EMIE_LMOUSE_PRESSED_DOWN && event.MouseInput.Event <= irr::EMIE_MMOUSE_PRESSED_DOWN ) {
				irr::u32 clicks = dev->checkSuccessiveClicks(event.MouseInput.X, event.MouseInput.Y, event.MouseInput.Event);
				if ( clicks == 2 ) {
					event.MouseInput.Event = (irr::EMOUSE_INPUT_EVENT)(irr::EMIE_LMOUSE_DOUBLE_CLICK + event.MouseInput.Event-irr::EMIE_LMOUSE_PRESSED_DOWN);
					dev->postEventFromUser(event);
				}
				else if ( clicks == 3 ) {
					event.MouseInput.Event = (irr::EMOUSE_INPUT_EVENT)(irr::EMIE_LMOUSE_TRIPLE_CLICK + event.MouseInput.Event-irr::EMIE_LMOUSE_PRESSED_DOWN);
					dev->postEventFromUser(event);
				}
			}
		}
		return 0;
	}

	switch (message) {
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			BYTE allKeys[256];

			event.EventType = irr::EET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (irr::EKEY_CODE)wParam;
			event.KeyInput.PressedDown = (message==WM_KEYDOWN || message == WM_SYSKEYDOWN);

			const UINT MY_MAPVK_VSC_TO_VK_EX = 3; // MAPVK_VSC_TO_VK_EX should be in SDK according to MSDN, but isn't in mine.
			if ( event.KeyInput.Key == irr::KEY_SHIFT ) {
				// this will fail on systems before windows NT/2000/XP, not sure _what_ will return there instead.
				event.KeyInput.Key = (irr::EKEY_CODE)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
			}
			if ( event.KeyInput.Key == irr::KEY_CONTROL ) {
				event.KeyInput.Key = (irr::EKEY_CODE)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
				// some keyboards will just return LEFT for both - left and right keys. So also check extend bit.
				if (lParam & 0x1000000)
					event.KeyInput.Key = irr::KEY_RCONTROL;
			}
			if ( event.KeyInput.Key == irr::KEY_MENU ) {
				event.KeyInput.Key = (irr::EKEY_CODE)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
				if (lParam & 0x1000000)
					event.KeyInput.Key = irr::KEY_RMENU;
			}

			GetKeyboardState(allKeys);

			event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);

			// Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
			// Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
			WORD keyChars[2];
			UINT scanCode = HIWORD(lParam);
			int conversionResult = ToAsciiEx(static_cast<UINT>(wParam),scanCode,allKeys,keyChars,0,KEYBOARD_INPUT_HKL);
			if (conversionResult == 1) {
				WORD unicodeChar;
				MultiByteToWideChar(
						KEYBOARD_INPUT_CODEPAGE,
						MB_PRECOMPOSED, // default
						(LPCSTR)keyChars,
						sizeof(keyChars),
						(WCHAR*)&unicodeChar,
						1 );
				event.KeyInput.Char = unicodeChar;
			}
			else
				event.KeyInput.Char = 0;

			// allow composing characters like '@' with Alt Gr on non-US keyboards
			if ((allKeys[VK_MENU] & 0x80) != 0)
				event.KeyInput.Control = 0;

			dev = getDeviceFromHWnd(hWnd);
			if (dev)
				dev->postEventFromUser(event);

			if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
				return DefWindowProc(hWnd, message, wParam, lParam);
			else
				return 0;
		}

	case WM_SIZE:
		{
			// resize
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
				dev->OnResized();
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SYSCOMMAND:
		// prevent screensaver or monitor powersave mode from starting
		if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
			(wParam & 0xFFF0) == SC_MONITORPOWER ||
			(wParam & 0xFFF0) == SC_KEYMENU
			)
			return 0;

		break;

	case WM_USER:
		event.EventType = irr::EET_USER_EVENT;
		event.UserEvent.UserData1 = static_cast<size_t>(wParam);
		event.UserEvent.UserData2 = static_cast<size_t>(lParam);
		dev = getDeviceFromHWnd(hWnd);

		if (dev)
			dev->postEventFromUser(event);

		return 0;

	case WM_SETCURSOR:
		// because Windows forgot about that in the meantime
		dev = getDeviceFromHWnd(hWnd);
		if (dev) {
			dev->getCursorControl()->setActiveIcon( dev->getCursorControl()->getActiveIcon() );
			dev->getCursorControl()->setVisible( dev->getCursorControl()->isVisible() );
		}
		break;

	case WM_INPUTLANGCHANGE:
		// get the new codepage used for keyboard input
		KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
		KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage( LOWORD(KEYBOARD_INPUT_HKL) );
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

namespace irr {

//! constructor
CIrrDeviceWin32::CIrrDeviceWin32(const SIrrlichtCreationParameters& params)
: CIrrDeviceStub(params), HWnd(0), Resized(false),
	ExternalWindow(false), Win32CursorControl(0) {
	#ifdef _DEBUG
	setDebugName("CIrrDeviceWin32");
	#endif

	// get windows version and create OS operator
	core::stringc winversion;
	getWindowsVersion(winversion);
	Operator = new COSOperator(winversion);
	os::Printer::log(winversion.c_str(), ELL_INFORMATION);

	// get handle to exe file
	HINSTANCE hInstance = GetModuleHandle(0);

	// create the window if we need to and we do not use the null device
	if (!CreationParams.WindowId && CreationParams.DriverType != video::EDT_NULL) {
		const fschar_t* ClassName = __TEXT("CIrrDeviceWin32");

		// Register Class
		WNDCLASSEX wcex;
		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= NULL;
		wcex.hCursor		= 0; // LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= ClassName;
		wcex.hIconSm		= 0;

		// if there is an icon, load it
		wcex.hIcon = (HICON)LoadImage(hInstance, __TEXT("irrlicht.ico"), IMAGE_ICON, 0,0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

		RegisterClassEx(&wcex);

		// calculate client size

		RECT clientSize;
		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = CreationParams.WindowSize.Width;
		clientSize.bottom = CreationParams.WindowSize.Height;

		DWORD style = getWindowStyle(CreationParams.Fullscreen, CreationParams.WindowResizable);
		AdjustWindowRect(&clientSize, style, FALSE);

		const s32 realWidth = clientSize.right - clientSize.left;
		const s32 realHeight = clientSize.bottom - clientSize.top;

		s32 windowLeft = (CreationParams.WindowPosition.X == -1 ?
							 (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2 :
							 CreationParams.WindowPosition.X);
		s32 windowTop = (CreationParams.WindowPosition.Y == -1 ?
							 (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2 :
							 CreationParams.WindowPosition.Y);

		if ( windowLeft < 0 )
			windowLeft = 0;
		if ( windowTop < 0 )
			windowTop = 0;	// make sure window menus are in screen on creation

		if (CreationParams.Fullscreen) {
			windowLeft = 0;
			windowTop = 0;
		}

		// create window
		HWnd = CreateWindow( ClassName, __TEXT(""), style, windowLeft, windowTop,
					realWidth, realHeight, NULL, NULL, hInstance, NULL);
		if (!HWnd) {
			os::Printer::log("Window could not be created.", ELL_ERROR);
		}

		CreationParams.WindowId = HWnd;
//		CreationParams.WindowSize.Width = realWidth;
//		CreationParams.WindowSize.Height = realHeight;

		ShowWindow(HWnd, SW_SHOWNORMAL);
		UpdateWindow(HWnd);

		// fix ugly ATI driver bugs. Thanks to ariaci
		MoveWindow(HWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);

		// make sure everything gets updated to the real sizes
		Resized = true;
	}
	else if (CreationParams.WindowId) {
		// attach external window
		HWnd = static_cast<HWND>(CreationParams.WindowId);
		RECT r;
		GetWindowRect(HWnd, &r);
		CreationParams.WindowSize.Width = r.right - r.left;
		CreationParams.WindowSize.Height = r.bottom - r.top;
		CreationParams.Fullscreen = false;
		ExternalWindow = true;
	}

	// create cursor control

	Win32CursorControl = new CCursorControl(this, CreationParams.WindowSize, HWnd, CreationParams.Fullscreen);
	CursorControl = Win32CursorControl;

	// initialize doubleclicks with system values
	MouseMultiClicks.DoubleClickTime = GetDoubleClickTime();

	// create driver

	createDriver();

	if (VideoDriver)
		createGUIAndScene();

	// register environment

	SEnvMapper em;
	em.irrDev = this;
	em.hWnd = HWnd;
	EnvMap.push_back(em);

	// set this as active window
	if (!ExternalWindow) {
		SetActiveWindow(HWnd);
		SetForegroundWindow(HWnd);
	}

	// get the codepage used for keyboard input
	KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
	KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage( LOWORD(KEYBOARD_INPUT_HKL) );

	// inform driver about the window size etc.
	resizeIfNecessary();
}

//! destructor
CIrrDeviceWin32::~CIrrDeviceWin32() {
	// unregister environment
	for (u32 i=0; i< EnvMap.size(); ++i) {
		if (EnvMap[i].hWnd == HWnd) {
			EnvMap.erase(i);
			break;
		}
	}
}

//! create the driver
void CIrrDeviceWin32::createDriver() {
	switch(CreationParams.DriverType) {
	case video::EDT_OPENGL:
#ifdef _IRR_COMPILE_WITH_OPENGL_
		switchToFullScreen();

		ContextManager = new video::CWGLManager();
		ContextManager->initialize(CreationParams, video::SExposedVideoData(HWnd));

		VideoDriver = video::createOpenGLDriver(CreationParams, FileSystem, ContextManager);

		if (!VideoDriver)
			os::Printer::log("Could not create OpenGL driver.", ELL_ERROR);
#else
		os::Printer::log("OpenGL driver was not compiled in.", ELL_ERROR);
#endif
		break;
	case video::EDT_NULL:
		VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
		break;
	default:
		os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
		break;
	}
}

//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceWin32::run() {
	os::Timer::tick();

	static_cast<CCursorControl*>(CursorControl)->update();

	handleSystemMessages();

	if (!Close)
		resizeIfNecessary();

	return !Close;
}

//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CIrrDeviceWin32::yield() {
	Sleep(1);
}

//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDeviceWin32::sleep(u32 timeMs, bool pauseTimer) {
	const bool wasStopped = Timer ? Timer->isStopped() : true;
	if (pauseTimer && !wasStopped)
		Timer->stop();

	Sleep(timeMs);

	if (pauseTimer && !wasStopped)
		Timer->start();
}

void CIrrDeviceWin32::resizeIfNecessary() {
	if (!Resized || !getVideoDriver())
		return;

	RECT r;
	GetClientRect(HWnd, &r);

	char tmp[255];

	if (r.right < 2 || r.bottom < 2) {
		sprintf(tmp, "Ignoring resize operation to (%ld %ld)", r.right, r.bottom);
		os::Printer::log(tmp);
	} else {
		sprintf(tmp, "Resizing window (%ld %ld)", r.right, r.bottom);
		os::Printer::log(tmp);

		getVideoDriver()->OnResize(irr::core::dimension2du((u32)r.right, (u32)r.bottom));
		getWin32CursorControl()->OnResize(getVideoDriver()->getScreenSize());
	}

	Resized = false;
}

DWORD CIrrDeviceWin32::getWindowStyle(bool fullscreen, bool resizable) const {
	if ( fullscreen )
		return WS_POPUP;

	if ( resizable )
		return WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	return WS_BORDER | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
}

//! sets the caption of the window
void CIrrDeviceWin32::setWindowCaption(const wchar_t* text) {
	// We use SendMessage instead of SetText to ensure proper
	// function even in cases where the HWND was created in a different thread
	DWORD_PTR dwResult;
	SendMessageTimeoutW(HWnd, WM_SETTEXT, 0,
			reinterpret_cast<LPARAM>(text),
			SMTO_ABORTIFHUNG, 2000, &dwResult);
}

//! notifies the device that it should close itself
void CIrrDeviceWin32::closeDevice() {
	if (!ExternalWindow) {
		MSG msg;
		PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
		PostQuitMessage(0);
		PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
		DestroyWindow(HWnd);
		const fschar_t* ClassName = __TEXT("CIrrDeviceWin32");
		HINSTANCE hInstance = GetModuleHandle(0);
		UnregisterClass(ClassName, hInstance);
	}
	Close=true;
}

//! returns if window is active. if not, nothing needs to be drawn
bool CIrrDeviceWin32::isWindowActive() const {
	return (GetActiveWindow() == HWnd);
}

//! returns if window has focus
bool CIrrDeviceWin32::isWindowFocused() const {
	bool ret = (GetFocus() == HWnd);
	return ret;
}

//! switches to fullscreen
bool CIrrDeviceWin32::switchToFullScreen() {
	if (!CreationParams.Fullscreen)
		return true;

	// No border, title bar, etc. is already set up through getWindowStyle()
	// We only set the window size to match the monitor.

	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	if (GetMonitorInfo(MonitorFromWindow(HWnd,MONITOR_DEFAULTTOPRIMARY),&mi)) {
		UINT flags = SWP_NOCOPYBITS|SWP_NOOWNERZORDER|SWP_FRAMECHANGED;
		SetWindowPos(HWnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top, flags);
	} else {
		CreationParams.Fullscreen = false;
	}

	return CreationParams.Fullscreen;
}

//! returns the win32 cursor control
CIrrDeviceWin32::CCursorControl* CIrrDeviceWin32::getWin32CursorControl() {
	return Win32CursorControl;
}

typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
// Needed for old windows apis
// depending on the SDK version and compilers some defines might be available
// or not
#ifndef PRODUCT_ULTIMATE
#define PRODUCT_ULTIMATE	0x00000001
#define PRODUCT_HOME_BASIC	0x00000002
#define PRODUCT_HOME_PREMIUM	0x00000003
#define PRODUCT_ENTERPRISE	0x00000004
#define PRODUCT_HOME_BASIC_N	0x00000005
#define PRODUCT_BUSINESS	0x00000006
#define PRODUCT_STARTER		0x0000000B
#endif
#ifndef PRODUCT_ULTIMATE_N
#define PRODUCT_BUSINESS_N	0x00000010
#define PRODUCT_HOME_PREMIUM_N	0x0000001A
#define PRODUCT_ENTERPRISE_N	0x0000001B
#define PRODUCT_ULTIMATE_N	0x0000001C
#endif
#ifndef PRODUCT_STARTER_N
#define PRODUCT_STARTER_N	0x0000002F
#endif
#ifndef PRODUCT_PROFESSIONAL
#define PRODUCT_PROFESSIONAL	0x00000030
#define PRODUCT_PROFESSIONAL_N	0x00000031
#endif
#ifndef PRODUCT_ULTIMATE_E
#define PRODUCT_STARTER_E	0x00000042
#define PRODUCT_HOME_BASIC_E	0x00000043
#define PRODUCT_HOME_PREMIUM_E	0x00000044
#define PRODUCT_PROFESSIONAL_E	0x00000045
#define PRODUCT_ENTERPRISE_E	0x00000046
#define PRODUCT_ULTIMATE_E	0x00000047
#endif

void CIrrDeviceWin32::getWindowsVersion(core::stringc& out) {
	OSVERSIONINFOEX osvi;
	PGPI pGPI;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
	if (!bOsVersionInfoEx) {
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (! GetVersionEx((OSVERSIONINFO *) &osvi))
			return;
	}

	switch (osvi.dwPlatformId) {
	case VER_PLATFORM_WIN32_NT:
		if (osvi.dwMajorVersion <= 4)
			out.append("Microsoft Windows NT ");
		else
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			out.append("Microsoft Windows 2000 ");
		else
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
			out.append("Microsoft Windows XP ");
		else
		if (osvi.dwMajorVersion == 6 ) {
			if (osvi.dwMinorVersion == 0) {
				if (osvi.wProductType == VER_NT_WORKSTATION)
					out.append("Microsoft Windows Vista ");
				else
					out.append("Microsoft Windows Server 2008 ");
			}
			else if (osvi.dwMinorVersion == 1) {
				if (osvi.wProductType == VER_NT_WORKSTATION)
					out.append("Microsoft Windows 7 ");
				else
					out.append("Microsoft Windows Server 2008 R2 ");
			}
			else if (osvi.dwMinorVersion == 2) {
				out.append("Microsoft Windows 8 or later ");
			}
		}

		if (bOsVersionInfoEx) {
			if (osvi.dwMajorVersion == 6) {
				DWORD dwType;
				pGPI = (PGPI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
				pGPI(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

				switch (dwType) {
				case PRODUCT_ULTIMATE:
				case PRODUCT_ULTIMATE_E:
				case PRODUCT_ULTIMATE_N:
					out.append("Ultimate Edition ");
					break;
				case PRODUCT_PROFESSIONAL:
				case PRODUCT_PROFESSIONAL_E:
				case PRODUCT_PROFESSIONAL_N:
					out.append("Professional Edition ");
					break;
				case PRODUCT_HOME_BASIC:
				case PRODUCT_HOME_BASIC_E:
				case PRODUCT_HOME_BASIC_N:
					out.append("Home Basic Edition ");
					break;
				case PRODUCT_HOME_PREMIUM:
				case PRODUCT_HOME_PREMIUM_E:
				case PRODUCT_HOME_PREMIUM_N:
					out.append("Home Premium Edition ");
					break;
				case PRODUCT_ENTERPRISE:
				case PRODUCT_ENTERPRISE_E:
				case PRODUCT_ENTERPRISE_N:
					out.append("Enterprise Edition ");
					break;
				case PRODUCT_BUSINESS:
				case PRODUCT_BUSINESS_N:
					out.append("Business Edition ");
					break;
				case PRODUCT_STARTER:
				case PRODUCT_STARTER_E:
				case PRODUCT_STARTER_N:
					out.append("Starter Edition ");
					break;
				}
			}
#ifdef VER_SUITE_ENTERPRISE
			else
			if (osvi.wProductType == VER_NT_WORKSTATION) {
#ifndef __BORLANDC__
				if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
					out.append("Personal ");
				else
					out.append("Professional ");
#endif
			}
			else if (osvi.wProductType == VER_NT_SERVER) {
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					out.append("DataCenter Server ");
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					out.append("Advanced Server ");
				else
					out.append("Server ");
			}
#endif
		} else {
			HKEY hKey;
			char szProductType[80];
			DWORD dwBufLen;

			RegOpenKeyEx( HKEY_LOCAL_MACHINE,
					__TEXT("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
					0, KEY_QUERY_VALUE, &hKey );
			RegQueryValueEx( hKey, __TEXT("ProductType"), NULL, NULL,
					(LPBYTE) szProductType, &dwBufLen);
			RegCloseKey( hKey );

			if (irr::core::stringc("WINNT").equals_ignore_case(szProductType))
				out.append("Professional ");
			if (irr::core::stringc("LANMANNT").equals_ignore_case(szProductType))
				out.append("Server ");
			if (irr::core::stringc("SERVERNT").equals_ignore_case(szProductType))
				out.append("Advanced Server ");
		}

		// Display version, service pack (if any), and build number.

		char tmp[255];

		if (osvi.dwMajorVersion <= 4 ) {
			sprintf(tmp, "version %lu.%lu %s (Build %lu)",
					osvi.dwMajorVersion,
					osvi.dwMinorVersion,
					irr::core::stringc(osvi.szCSDVersion).c_str(),
					osvi.dwBuildNumber & 0xFFFF);
		} else {
			sprintf(tmp, "%s (Build %lu)", irr::core::stringc(osvi.szCSDVersion).c_str(),
			osvi.dwBuildNumber & 0xFFFF);
		}

		out.append(tmp);
		break;

	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) {
			out.append("Microsoft Windows 95 ");
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				out.append("OSR2 " );
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) {
			out.append("Microsoft Windows 98 ");
			if ( osvi.szCSDVersion[1] == 'A' )
				out.append( "SE " );
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			out.append("Microsoft Windows Me ");

		break;

	case VER_PLATFORM_WIN32s:
		out.append("Microsoft Win32s ");
		break;
	}
}

//! Notifies the device, that it has been resized
void CIrrDeviceWin32::OnResized() {
	Resized = true;
}

//! Resize the render window.
void CIrrDeviceWin32::setWindowSize(const irr::core::dimension2d<u32>& size) {
	if (ExternalWindow || !getVideoDriver() || CreationParams.Fullscreen)
		return;

	// get size of the window for the give size of the client area
	DWORD style = static_cast<DWORD>(GetWindowLongPtr(HWnd, GWL_STYLE));
	DWORD exStyle = static_cast<DWORD>(GetWindowLongPtr(HWnd, GWL_EXSTYLE));
	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = size.Width;
	clientSize.bottom = size.Height;
	AdjustWindowRectEx(&clientSize, style, false, exStyle);
	const s32 realWidth = clientSize.right - clientSize.left;
	const s32 realHeight = clientSize.bottom - clientSize.top;

	UINT flags = SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER;
	SetWindowPos(HWnd, HWND_TOP, 0, 0, realWidth, realHeight, flags);
}

//! Sets if the window should be resizable in windowed mode.
void CIrrDeviceWin32::setResizable(bool resize) {
	if (ExternalWindow || !getVideoDriver() || CreationParams.Fullscreen)
		return;

	LONG_PTR style = (LONG_PTR)getWindowStyle(false, resize);
	if (!SetWindowLongPtr(HWnd, GWL_STYLE, style))
		os::Printer::log("Could not change window style.");

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = getVideoDriver()->getScreenSize().Width;
	clientSize.bottom = getVideoDriver()->getScreenSize().Height;

	AdjustWindowRect(&clientSize, static_cast<DWORD>(style), FALSE);

	const s32 realWidth = clientSize.right - clientSize.left;
	const s32 realHeight = clientSize.bottom - clientSize.top;

	const s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	const s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	SetWindowPos(HWnd, HWND_TOP, windowLeft, windowTop, realWidth, realHeight,
		SWP_FRAMECHANGED | SWP_NOMOVE | SWP_SHOWWINDOW);

	static_cast<CCursorControl*>(CursorControl)->updateBorderSize(CreationParams.Fullscreen, resize);
}

core::position2di CIrrDeviceWin32::getWindowPosition() {
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	if (GetWindowPlacement(HWnd, &wndpl)) {
		return core::position2di((int)wndpl.rcNormalPosition.left,
								 (int)wndpl.rcNormalPosition.top);
	} else {
		// No reason for this to happen
		os::Printer::log("Failed to retrieve window location", ELL_ERROR);
		return core::position2di(-1, -1);
	}
}

//! Process system events
void CIrrDeviceWin32::handleSystemMessages() {
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (ExternalWindow && msg.hwnd == HWnd) {
			if (msg.hwnd == HWnd) {
				WndProc(HWnd, msg.message, msg.wParam, msg.lParam);
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			// No message translation because we don't use WM_CHAR and it would conflict with our
			// deadkey handling.
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			Close = true;
	}
}

// Same function Windows offers in VersionHelpers.h, but we can't use that as it's not available in older sdk's (minimum is SDK 8.1)
bool CIrrDeviceWin32::isWindowsVistaOrGreater() {
#if (_WIN32_WINNT >= 0x0500)
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6; //  Windows Vista

	if ( !GetVersionEx((OSVERSIONINFO*)&osvi) ) {
		return false;
	}

	return VerifyVersionInfo(&osvi, VER_MAJORVERSION, VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL)) ? true : false;
#else
	return false;
#endif
}

CIrrDeviceWin32::CCursorControl::CCursorControl(CIrrDeviceWin32* device, const core::dimension2d<u32>& wsize, HWND hwnd, bool fullscreen)
	: Device(device), WindowSize(wsize), InvWindowSize(0.0f, 0.0f),
		HWnd(hwnd), BorderX(0), BorderY(0),
		UseReferenceRect(false), IsVisible(true)
		, ActiveIcon(gui::ECI_NORMAL), ActiveIconStartTime(0) {
	if (WindowSize.Width!=0)
		InvWindowSize.Width = 1.0f / WindowSize.Width;

	if (WindowSize.Height!=0)
		InvWindowSize.Height = 1.0f / WindowSize.Height;

	updateBorderSize(fullscreen, false);
	initCursors();
}

CIrrDeviceWin32::CCursorControl::~CCursorControl() {
	for ( u32 i=0; i < Cursors.size(); ++i ) {
		for ( u32 f=0; f < Cursors[i].Frames.size(); ++f ) {
			DestroyCursor(Cursors[i].Frames[f].IconHW);
		}
	}
}

void CIrrDeviceWin32::CCursorControl::initCursors() {
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_ARROW)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_CROSS)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_HAND)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_HELP)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_IBEAM)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_NO)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_WAIT)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_SIZEALL)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_SIZENESW)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_SIZENWSE)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_SIZENS)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_SIZEWE)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_UPARROW)) );
}

void CIrrDeviceWin32::CCursorControl::update() {
	if ( !Cursors[ActiveIcon].Frames.empty() && Cursors[ActiveIcon].FrameTime ) {
		// update animated cursors. This could also be done by X11 in case someone wants to figure that out (this way was just easier to implement)
		u32 now = Device->getTimer()->getRealTime();
		u32 frame = ((now - ActiveIconStartTime) / Cursors[ActiveIcon].FrameTime) % Cursors[ActiveIcon].Frames.size();
		SetCursor( Cursors[ActiveIcon].Frames[frame].IconHW );
	}
}

//! Sets the active cursor icon
void CIrrDeviceWin32::CCursorControl::setActiveIcon(gui::ECURSOR_ICON iconId) {
	if ( iconId >= (s32)Cursors.size() )
		return;

	ActiveIcon = iconId;
	ActiveIconStartTime = Device->getTimer()->getRealTime();
	if ( Cursors[ActiveIcon].Frames.size() )
		SetCursor( Cursors[ActiveIcon].Frames[0].IconHW );
}

//! Return a system-specific size which is supported for cursors. Larger icons will fail, smaller icons might work.
core::dimension2di CIrrDeviceWin32::CCursorControl::getSupportedIconSize() const {
	core::dimension2di result;

	result.Width = GetSystemMetrics(SM_CXCURSOR);
	result.Height = GetSystemMetrics(SM_CYCURSOR);

	return result;
}

} // end namespace

#endif // _IRR_COMPILE_WITH_WINDOWS_DEVICE_
