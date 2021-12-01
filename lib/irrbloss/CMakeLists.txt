cmake_minimum_required(VERSION 3.5)

project(Irrlicht
	VERSION 1.9.0
	LANGUAGES CXX
)

message(STATUS "*** Building Irrbloss ${PROJECT_VERSION} ***")

include(GNUInstallDirs)

if(APPLE)
	set(sysname OSX)
elseif(MSVC)
	set(sysname Win32-VisualStudio)
elseif(WIN32)
	set(sysname Win32-gcc)
else()
	set(sysname Linux)
endif()
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/${sysname})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${sysname})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
add_subdirectory(source)

# Installation of headers.
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/irrbloss"
)

# Installation of CMake target and configuration files.
install(EXPORT Irrbloss-export
	FILE IrrblossTargets.cmake
	NAMESPACE Irrbloss::
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Irrbloss"
)

include(CMakePackageConfigHelpers)
configure_package_config_file("${PROJECT_SOURCE_DIR}/Config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/IrrblossConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Irrbloss"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/IrrblossConfigVersion.cmake"
	COMPATIBILITY AnyNewerVersion
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/IrrblossConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/IrrblossConfigVersion.cmake"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Irrbloss"
)
