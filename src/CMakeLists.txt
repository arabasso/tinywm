add_library(tinywm INTERFACE)

if (APPLE)
set (SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tinywm.h ${CMAKE_CURRENT_SOURCE_DIR}/tinywm.m)
else ()
set (SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tinywm.h ${CMAKE_CURRENT_SOURCE_DIR}/tinywm.c)
endif()

target_sources(tinywm INTERFACE ${SOURCES})

target_include_directories(tinywm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
	target_compile_definitions(tinywm INTERFACE TWM_WIN32)
	target_link_libraries (tinywm INTERFACE winmm Shcore dwmapi)
elseif (APPLE)
	target_compile_definitions(tinywm INTERFACE TWM_COCOA)
	target_link_libraries (tinywm INTERFACE "-framework Cocoa" "-framework Carbon" "-framework CoreGraphics" "-framework IOKit" "-framework ApplicationServices")
elseif (UNIX)
	target_compile_definitions(tinywm INTERFACE TWM_X11)
	target_link_libraries (tinywm INTERFACE X11 Xi Xrandr)
endif()
