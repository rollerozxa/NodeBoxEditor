add_definitions(-DIRRLICHT_EXPORTS=1)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-D_DEBUG)
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

# Compiler flags

if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")

	add_compile_options(-Wall -pipe -fno-exceptions -fno-rtti)
elseif(MSVC)
	string(APPEND CMAKE_CXX_STANDARD_LIBRARIES " msvcrt.lib") # ???? fuck off

	add_compile_options(/GR- /Zl)
endif()

# Required libs

find_package(PNG REQUIRED)

# To configure the features available in this Irrlicht build please edit
# include/IrrCompileConfig.h and re-run CMake from a clean state
include(CheckSymbolExists)
set(CMAKE_REQUIRED_INCLUDES ${PROJECT_SOURCE_DIR}/include)
unset(OGL_ENABLED CACHE)

check_symbol_exists(_IRR_COMPILE_WITH_OPENGL_ "IrrCompileConfig.h" OGL_ENABLED)
if(OGL_ENABLED)
	set(OpenGL_GL_PREFERENCE "LEGACY")
	find_package(OpenGL REQUIRED)
endif()

# Platform-specific libs

if(APPLE)

else()
	# Unix probably
	find_package(X11 REQUIRED)
endif()

set(link_includes
	"${PROJECT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}"

	"${PNG_INCLUDE_DIR}"

	${OPENGL_INCLUDE_DIR}
	${EGL_INCLUDE_DIR}

	${X11_INCLUDE_DIR}
)

set(link_libs
	"${PNG_LIBRARY}"

	${OPENGL_LIBRARIES}
	${EGL_LIBRARY}

	"$<$<PLATFORM_ID:Android>:native_app_glue -landroid -llog>"
	${COCOA_LIB}
	${IOKIT_LIB}
	"$<$<PLATFORM_ID:Windows>:gdi32>"
	"$<$<PLATFORM_ID:Windows>:winmm>"
	${X11_X11_LIB}
	${X11_Xxf86vm_LIB}
)

# Source files

set(IRRMESHLOADER
	COBJMeshFileLoader.cpp
)

add_library(IRRMESHOBJ OBJECT
	CMeshSceneNode.cpp
	CAnimatedMeshSceneNode.cpp
	${IRRMESHLOADER}
)

add_library(IRROBJ OBJECT
	CCameraSceneNode.cpp
	CEmptySceneNode.cpp
	CGeometryCreator.cpp
	CLightSceneNode.cpp
	CMeshManipulator.cpp
	CMetaTriangleSelector.cpp
	COctreeTriangleSelector.cpp
	CSceneCollisionManager.cpp
	CSceneManager.cpp
	CCubeSceneNode.cpp
	CTriangleSelector.cpp
	CMeshCache.cpp
)

set(IRRDRVROBJ
	CNullDriver.cpp
	COpenGLCacheHandler.cpp
	COpenGLDriver.cpp
	COpenGLShaderMaterialRenderer.cpp
	COpenGLSLMaterialRenderer.cpp
	COpenGLExtensionHandler.cpp
	CGLXManager.cpp
	CWGLManager.cpp
)

set(IRRIMAGEOBJ
	CColorConverter.cpp
	CImage.cpp
	CImageLoaderPNG.cpp
	CImageWriterPNG.cpp
)

add_library(IRRVIDEOOBJ OBJECT
	CFPSCounter.cpp
	${IRRDRVROBJ}
	${IRRIMAGEOBJ}
)


add_library(IRRIOOBJ OBJECT
	CFileList.cpp
	CFileSystem.cpp
	CMemoryFile.cpp
	CReadFile.cpp
	CWriteFile.cpp
)

add_library(IRROTHEROBJ OBJECT
	CIrrDeviceLinux.cpp
	CIrrDeviceStub.cpp
	CIrrDeviceWin32.cpp
	CLogger.cpp
	COSOperator.cpp
	Irrlicht.cpp
	os.cpp
)

add_library(IRRGUIOBJ OBJECT
	CGUIButton.cpp
	CGUICheckBox.cpp
	CGUIComboBox.cpp
	CGUIContextMenu.cpp
	CGUIEditBox.cpp
	CGUIEnvironment.cpp
	CGUIFont.cpp
	CGUIImage.cpp
	CGUIListBox.cpp
	CGUIMenu.cpp
	CGUIMessageBox.cpp
	CGUIModalScreen.cpp
	CGUIScrollBar.cpp
	CGUISkin.cpp
	CGUIStaticText.cpp
	CGUIToolBar.cpp
	CGUIWindow.cpp
	CGUISpriteBank.cpp
	CGUIImageList.cpp
)

# Library

option(BUILD_SHARED_LIBS "Build shared library" TRUE)

add_library(Irrbloss)
foreach(object_lib
	IRRMESHOBJ IRROBJ IRRVIDEOOBJ
	IRRIOOBJ IRROTHEROBJ IRRGUIOBJ)
	# Set include directories for object library compilation
	target_include_directories(${object_lib} PRIVATE ${link_includes})
	# Add objects from object library to main library
	target_sources(Irrbloss PRIVATE $<TARGET_OBJECTS:${object_lib}>)
endforeach()

# Alias target provides add_submodule compatibility
add_library(Irrbloss::Irrbloss ALIAS Irrbloss)

target_include_directories(Irrbloss
	PUBLIC
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/irrbloss>"
	PRIVATE
		${link_includes}
)

target_link_libraries(Irrbloss PRIVATE ${link_libs})

set_target_properties(Irrbloss PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(WIN32)
	set_target_properties(Irrbloss PROPERTIES PREFIX "") # for DLL name
endif()

# Installation of library
set(INSTALL_TARGETS Irrbloss)

install(TARGETS ${INSTALL_TARGETS}
	EXPORT Irrbloss-export
	DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
