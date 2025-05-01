if (HAVE_VULKAN1)
	get_filename_component (DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	set (EXE_NAME ${DIR_NAME})

	file (GLOB SOURCES *.h *.c *.cpp)

	add_executable (${EXE_NAME} ${SOURCES})

	target_compile_definitions(${EXE_NAME} PRIVATE TWM_VULKAN)

	target_link_libraries (${EXE_NAME} tinywm Vulkan::Vulkan vk-bootstrap)

	if (APPLE)
		target_link_libraries (${EXE_NAME} "-framework QuartzCore")
	endif()

	install(TARGETS ${EXE_NAME}
		BUNDLE DESTINATION ${INSTALL_PATH}
		RUNTIME DESTINATION ${INSTALL_PATH}
		LIBRARY DESTINATION ${INSTALL_PATH}
		ARCHIVE DESTINATION ${INSTALL_PATH})
endif ()
