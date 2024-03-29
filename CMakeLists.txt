#
# Set up project
#
cmake_minimum_required(VERSION 2.6)
project(nodeboxeditor)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

#
# Configuration
#
set(NBE_MAJOR_VERSION 0)
set(NBE_MINOR_VERSION 9)
set(NBE_PATCH_VERSION 0)
set(NBE_LABEL_VERSION "Obsidian Glass")
option(DEBUG "Debug mode" 0)
if(DEBUG)
	message("-- Is debug")
	set(IS_DEBUG true)
endif(DEBUG)
configure_file(
	"${PROJECT_SOURCE_DIR}/src/conf_cmake.hpp.in"
	"${PROJECT_BINARY_DIR}/conf_cmake.hpp"
)


#
# Source files
#
set(NBE_SRC
	src/main.cpp
	src/Configuration.cpp
	src/GUIHelpers.cpp
	src/EditorState.cpp
	src/MenuState.cpp
	src/Editor.cpp
	src/minetest.cpp

	src/project/project.cpp
	src/project/media.cpp
	src/project/node.cpp
	src/project/nodebox.cpp

	src/modes/NBEditor.cpp
	src/modes/NodeEditor.cpp
	src/modes/TextureEditor.cpp

	src/dialogs/FileDialog.cpp
	src/dialogs/TextureDialog.cpp
	src/dialogs/ImageDialog.cpp

	src/FileFormat/FileFormat.cpp
	src/FileFormat/helpers.cpp
	src/FileFormat/NBE.cpp
	src/FileFormat/Lua.cpp
	src/FileFormat/obj.cpp

	src/util/string.cpp
	src/util/filesys.cpp
	src/util/SimpleFileCombiner.cpp
	src/util/tinyfiledialogs.c
)
add_executable(${PROJECT_NAME} ${NBE_SRC})

#
# Dependencies
#

# tell Irrlicht (Irrbloss) to create a static library
if(WIN32)
	set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared library" FORCE)
else()
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared library" FORCE)
endif()
add_subdirectory(lib/irrbloss EXCLUDE_FROM_ALL)
unset(BUILD_SHARED_LIBS CACHE)

if(NOT TARGET Irrbloss)
	message(FATAL_ERROR "Irrbloss project is missing a CMake target?!")
endif()

if(WIN32)
	# Windows
	if(MSVC) # MSVC Specifics
		set(PLATFORM_LIBS dbghelp.lib ${PLATFORM_LIBS})
		# Surpress some useless warnings
		add_definitions ( /D "_CRT_SECURE_NO_DEPRECATE" /W1 )
	else() # Probably MinGW = GCC
		set(PLATFORM_LIBS "")
	endif()
	set(PLATFORM_LIBS ws2_32.lib shlwapi.lib ${PLATFORM_LIBS})
endif(WIN32)


find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
include_directories(
	${PROJECT_BINARY_DIR}
	${CMAKE_BUILD_TYPE}
	${X11_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${PNG_INCLUDE_DIR}
)
set(TLL
	Irrbloss::Irrbloss
	${X11_LIBRARIES}
	${OPENGL_LIBRARIES}
	${PNG_LIBRARIES}
)
if(UNIX)
	find_library(XXF86VM_LIBRARY Xxf86vm)
	target_link_libraries(
		${PROJECT_NAME}
		${TLL}
		${XXF86VM_LIBRARY}
	)
else()
	target_link_libraries(
		${PROJECT_NAME}
		${TLL}
	)
endif(UNIX)

#
# Executable
#
file(MAKE_DIRECTORY "bin")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -pthread")

#
# Installation
#
install(FILES   media/fontlucida.png        DESTINATION share/nodeboxeditor/media)
install(FILES   media/gui_scale.png         DESTINATION share/nodeboxeditor/media)
install(FILES   media/coordinates.png       DESTINATION share/nodeboxeditor/media)
install(FILES   media/icon_mode_node.png    DESTINATION share/nodeboxeditor/media)
install(FILES   media/icon_mode_nodebox.png DESTINATION share/nodeboxeditor/media)
install(FILES   media/icon_mode_texture.png DESTINATION share/nodeboxeditor/media)
install(FILES   media/sky.png               DESTINATION share/nodeboxeditor/media)
install(FILES   media/texture_box.png       DESTINATION share/nodeboxeditor/media)
install(FILES   media/texture_terrain.png   DESTINATION share/nodeboxeditor/media)
install(FILES   media/icon.png              DESTINATION share/nodeboxeditor/media)
install(FILES   media/rotate_x.png          DESTINATION share/nodeboxeditor/media)
install(FILES   media/rotate_y.png          DESTINATION share/nodeboxeditor/media)
install(FILES   media/rotate_z.png          DESTINATION share/nodeboxeditor/media)
install(FILES   media/flip_x.png          DESTINATION share/nodeboxeditor/media)
install(FILES   media/flip_y.png          DESTINATION share/nodeboxeditor/media)
install(FILES   media/flip_z.png          DESTINATION share/nodeboxeditor/media)
install(FILES   editor.conf.example         DESTINATION share/nodeboxeditor)
if(UNIX)
	install (FILES   nodeboxeditor.desktop       DESTINATION share/applications)
endif()
install(FILES   README.md                   DESTINATION share/nodeboxeditor)
install(TARGETS ${PROJECT_NAME}             DESTINATION bin)
if(WIN32)
	if(DEFINED IRRLICHT_DLL)
		install(FILES ${IRRLICHT_DLL} DESTINATION bin)
	endif()
endif()
