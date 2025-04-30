cmake_minimum_required(VERSION 3.14...3.27)

project (tinywm)

option(COMPILE_EXAMPLES "Compile examples." ON)

set (BUILD_SHARED_LIBS OFF)
set (CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set (CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_C_STANDARD 17)

if (WIN32)
	if (MSVC)
		add_definitions ("/MP")
	
		set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
		set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

		set (CMAKE_EXE_LINKER_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
		set (CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	else ()
		set (CMAKE_EXE_LINKER_FLAGS_RELEASE "-mwindows")
		set (CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "-mwindows")
	endif ()
endif ()

set (CMAKE_SKIP_BUILD_RPATH FALSE)
set (CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set (CMAKE_XCODE_GENERATE_SCHEME YES)

set (PREDEFINED_TARGETS_FOLDER CMakePredefinedTargets)

include (CheckIncludeFile)

find_package (OpenGL)
set (HAVE_OPENGL4 OpenGL_FOUND)
find_package (Vulkan)
set (HAVE_VULKAN1 Vulkan_FOUND)
check_include_file (d3d12.h HAVE_D3D12)
check_include_file (d3d11.h HAVE_D3D11)

if (APPLE)
	set (HAVE_METAL ON)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND COMPILE_EXAMPLES)
    add_subdirectory (3rdparty)
	add_subdirectory (src/examples)
endif()


add_subdirectory (src)
