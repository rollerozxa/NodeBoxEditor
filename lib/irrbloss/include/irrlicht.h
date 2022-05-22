/* irrlicht.h -- interface of the 'Irrlicht Engine'

  Copyright (C) 2002-2012 Nikolaus Gebhardt

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Please note that the Irrlicht Engine is based in part on the work of the
  Independent JPEG Group, the zlib and the libPng. This means that if you use
  the Irrlicht Engine in your product, you must acknowledge somewhere in your
  documentation that you've used the IJG code. It would also be nice to mention
  that you use the Irrlicht Engine, the zlib and libPng. See the README files
  in the jpeglib, the zlib and libPng for further information.
*/

#ifndef __IRRLICHT_H_INCLUDED__
#define __IRRLICHT_H_INCLUDED__

#include "IrrCompileConfig.h"
#include "aabbox3d.h"
#include "CDynamicMeshBuffer.h"
#include "CIndexBuffer.h"
#include "CMeshBuffer.h"
#include "coreutil.h"
#include "CVertexBuffer.h"
#include "dimension2d.h"
#include "ECullingTypes.h"
#include "EDebugSceneTypes.h"
#include "EDriverFeatures.h"
#include "EDriverTypes.h"
#include "EGUIAlignment.h"
#include "EGUIElementTypes.h"
#include "EHardwareBufferFlags.h"
#include "EMaterialFlags.h"
#include "EMaterialTypes.h"
#include "EMessageBoxFlags.h"
#include "ESceneNodeTypes.h"
#include "fast_atof.h"
#include "heapsort.h"
#include "IAnimatedMesh.h"
#include "IAnimatedMeshSceneNode.h"
#include "ICameraSceneNode.h"
#include "IContextManager.h"
#include "ICursorControl.h"
#include "IDynamicMeshBuffer.h"
#include "IEventReceiver.h"
#include "IFileList.h"
#include "IFileSystem.h"
#include "IGeometryCreator.h"
#include "IGPUProgrammingServices.h"
#include "IGUIButton.h"
#include "IGUICheckBox.h"
#include "IGUIComboBox.h"
#include "IGUIContextMenu.h"
#include "IGUIEditBox.h"
#include "IGUIElement.h"
#include "IGUIEnvironment.h"
#include "IGUIFont.h"
#include "IGUIFontBitmap.h"
#include "IGUIImage.h"
#include "IGUIListBox.h"
#include "IGUIScrollBar.h"
#include "IGUISkin.h"
#include "IGUISpriteBank.h"
#include "IGUIStaticText.h"
#include "IGUIToolbar.h"
#include "IGUIWindow.h"
#include "IImage.h"
#include "IImageLoader.h"
#include "IImageWriter.h"
#include "IIndexBuffer.h"
#include "ILightSceneNode.h"
#include "ILogger.h"
#include "IMaterialRenderer.h"
#include "IMaterialRendererServices.h"
#include "IMesh.h"
#include "IMeshBuffer.h"
#include "IMeshCache.h"
#include "IMeshLoader.h"
#include "IMeshManipulator.h"
#include "IMeshSceneNode.h"
#include "IMetaTriangleSelector.h"
#include "IOSOperator.h"
#include "IReadFile.h"
#include "IReferenceCounted.h"
#include "irrArray.h"
#include "IRenderTarget.h"
#include "IrrlichtDevice.h"
#include "irrList.h"
#include "irrMath.h"
#include "irrString.h"
#include "irrTypes.h"
#include "path.h"
#include "ISceneCollisionManager.h"
#include "ISceneManager.h"
#include "ISceneNode.h"
#include "IShaderConstantSetCallBack.h"
#include "IShadowVolumeSceneNode.h"
#include "ITexture.h"
#include "ITimer.h"
#include "ITriangleSelector.h"
#include "IVertexBuffer.h"
#include "IVideoDriver.h"
#include "IVideoModeList.h"
#include "IWriteFile.h"
#include "ILightManager.h"
#include "Keycodes.h"
#include "line2d.h"
#include "line3d.h"
#include "matrix4.h"
#include "plane3d.h"
#include "position2d.h"
#include "quaternion.h"
#include "rect.h"
#include "S3DVertex.h"
#include "SAnimatedMesh.h"
#include "SColor.h"
#include "SExposedVideoData.h"
#include "SIrrCreationParameters.h"
#include "SKeyMap.h"
#include "SLight.h"
#include "SMaterial.h"
#include "SMesh.h"
#include "SMeshBuffer.h"
#include "SMeshBufferLightMap.h"
#include "SSkinMeshBuffer.h"
#include "SVertexIndex.h"
#include "SViewFrustum.h"
#include "triangle3d.h"
#include "vector2d.h"
#include "vector3d.h"


#include "SIrrCreationParameters.h"

//! Everything in the Irrlicht Engine can be found in this namespace.
namespace irr {
	//! Creates an Irrlicht device. The Irrlicht device is the root object for using the engine.
	/** If you need more parameters to be passed to the creation of the Irrlicht Engine device,
	use the createDeviceEx() function.
	\param driverType: Type of the video driver to use. This can currently be video::EDT_NULL and video::EDT_OPENGL.
	\param windowSize: Size of the window or the video mode in fullscreen mode.
	\param bits: Bits per pixel in fullscreen mode. Ignored if windowed mode.
	\param fullscreen: Should be set to true if the device should run in fullscreen. Otherwise
		the device runs in windowed mode.
	\param stencilbuffer: Specifies if the stencil buffer should be enabled. Set this to true,
	if you want the engine be able to draw stencil buffer shadows. Note that not all
	devices are able to use the stencil buffer. If they don't no shadows will be drawn.
	\param vsync: Specifies vertical synchronization: If set to true, the driver will wait
	for the vertical retrace period, otherwise not.
	\param receiver: A user created event receiver.
	\return Returns pointer to the created IrrlichtDevice or null if the
	device could not be created.
	*/
	extern "C" IRRLICHT_API IrrlichtDevice* IRRCALLCONV createDevice(
		video::E_DRIVER_TYPE driverType = video::EDT_OPENGL,
		// parentheses are necessary for some compilers
		const core::dimension2d<u32>& windowSize = (core::dimension2d<u32>(640,480)),
		u32 bits = 32,
		bool fullscreen = false,
		bool stencilbuffer = true,
		bool vsync = false,
		IEventReceiver* receiver = 0);

	//! typedef for Function Pointer
	typedef IrrlichtDevice* (IRRCALLCONV *funcptr_createDevice )(
			video::E_DRIVER_TYPE driverType,
			const core::dimension2d<u32>& windowSize,
			u32 bits,
			bool fullscreen,
			bool stencilbuffer,
			bool vsync,
			IEventReceiver* receiver);


	//! Creates an Irrlicht device with the option to specify advanced parameters.
	/** Usually you should used createDevice() for creating an Irrlicht Engine device.
	Use this function only if you wish to specify advanced parameters like a window
	handle in which the device should be created.
	\param parameters: Structure containing advanced parameters for the creation of the device.
	See irr::SIrrlichtCreationParameters for details.
	\return Returns pointer to the created IrrlichtDevice or null if the
	device could not be created. */
	extern "C" IRRLICHT_API IrrlichtDevice* IRRCALLCONV createDeviceEx(
		const SIrrlichtCreationParameters& parameters);

	//! typedef for Function Pointer
	typedef IrrlichtDevice* (IRRCALLCONV *funcptr_createDeviceEx )( const SIrrlichtCreationParameters& parameters );


	// THE FOLLOWING IS AN EMPTY LIST OF ALL SUB NAMESPACES
	// EXISTING ONLY FOR THE DOCUMENTATION SOFTWARE DOXYGEN.

	//! Basic classes such as vectors, planes, arrays, lists, and so on can be found in this namespace.
	namespace core
	{
	}

	//! The gui namespace contains useful classes for easy creation of a graphical user interface.
	namespace gui
	{
	}

	//! This namespace provides interfaces for input/output: Reading and writing files, accessing zip archives, xml files, ...
	namespace io
	{
	}

	//! All scene management can be found in this namespace: Mesh loading, special scene nodes like octrees and billboards, ...
	namespace scene
	{
	}

	//! The video namespace contains classes for accessing the video driver. All 2d and 3d rendering is done here.
	namespace video
	{
	}
}

/*! \file irrlicht.h
	\brief Main header file of the irrlicht, the only file needed to include.
*/

#endif
