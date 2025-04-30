if (HAVE_D3D11)
	get_filename_component (DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	set (EXE_NAME ${DIR_NAME})

	file (GLOB SOURCES *.h *.c *.cpp)

	add_executable (${EXE_NAME} ${SOURCES})

	target_include_directories (${EXE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

	target_link_libraries (${EXE_NAME} tinywm d3d11 dxgi DirectX-Headers DirectX-Guids)
endif()
