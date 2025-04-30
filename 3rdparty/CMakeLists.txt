include(FetchContent)

# By using such 'if' we allow integration of our project
# with other projects, which use the **same approach**
# for disabling installation.
if (NOT DEFINED _DISABLE_INSTALLATION)
  # This variable is responsible for installation disabling.
  set (_DISABLE_INSTALLATION FALSE)

  # Replace install() with conditional installation.
  macro (install)
    if (NOT _DISABLE_INSTALLATION)
      _install (${ARGN})
    endif ()
  endmacro ()
endif ()

# Store old value of flag for the case the external project already disables installation of current one.
set (_DISABLE_INSTALLATION_OLD ${_DISABLE_INSTALLATION})
set (_DISABLE_INSTALLATION TRUE)

# GLAD
if (HAVE_OPENGL4)
	message(STATUS "glad enabled")
	set (CMAKE_FOLDER 3rdparty/glad)
	add_subdirectory (glad)
endif ()

# Metal
IF (APPLE)
	message(STATUS "Metal enabled")
	set (CMAKE_FOLDER 3rdparty/metal)
	add_subdirectory (metal)
ENDIF()

if (HAVE_D3D11 OR HAVE_D3D12)
	# DirectX-Headers
	FetchContent_Declare(
		fetch_directxheaders
		GIT_REPOSITORY  https://github.com/microsoft/DirectX-Headers
		GIT_TAG         v1.615.0
		GIT_PROGRESS    TRUE
		GIT_SHALLOW     ON
		UPDATE_COMMAND  ""
	)
	set (CMAKE_FOLDER 3rdparty/DirectX-Headers)
	set (CMAKE_MESSAGE_LOG_LEVEL WARNING)
	FetchContent_Populate (fetch_directxheaders)
	set (DXHEADERS_CMAKELISTS "${fetch_directxheaders_SOURCE_DIR}/CMakeLists.txt")
	file (READ "${DXHEADERS_CMAKELISTS}" DX_CMAKELISTS_CONTENTS)
	string (REPLACE "include(CTest)" "# include(CTest) (patched)" DX_CMAKELISTS_CONTENTS "${DX_CMAKELISTS_CONTENTS}")
	file (WRITE "${DXHEADERS_CMAKELISTS}" "${DX_CMAKELISTS_CONTENTS}")
	add_subdirectory (${fetch_directxheaders_SOURCE_DIR} ${fetch_directxheaders_BINARY_DIR})
	set (CMAKE_MESSAGE_LOG_LEVEL STATUS)
	message(STATUS "DirectX-Headers enabled")
endif ()

# Dear ImGUI
FetchContent_Declare(
	fetch_imgui
	GIT_REPOSITORY  https://github.com/ocornut/imgui
	GIT_TAG         v1.91.9b
	GIT_PROGRESS    TRUE
	GIT_SHALLOW     ON
	UPDATE_COMMAND  ""
)
FetchContent_GetProperties(fetch_imgui)
if (NOT fetch_imgui_POPULATED)
	FetchContent_Populate(fetch_imgui)

	set (CMAKE_FOLDER 3rdparty/imgui)
	FILE (GLOB IMGUI_SOURCES ${fetch_imgui_SOURCE_DIR}/*.h ${fetch_imgui_SOURCE_DIR}/*.cpp)
	add_library (imgui ${IMGUI_SOURCES})
	target_include_directories (imgui PUBLIC ${fetch_imgui_SOURCE_DIR})

	if (HAVE_OPENGL4)
		add_library (imgui_gl4_tinywm ${fetch_imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h ${fetch_imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
		target_link_libraries (imgui_gl4_tinywm imgui)
	endif ()

	if (HAVE_VULKAN1)
		add_library (imgui_vk1_tinywm ${fetch_imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.h ${fetch_imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)
		target_link_libraries (imgui_vk1_tinywm imgui Vulkan::Vulkan)
	endif ()

	if (HAVE_D3D11 OR HAVE_D3D12)
		add_library (imgui_d3d12_tinywm ${fetch_imgui_SOURCE_DIR}/backends/imgui_impl_dx12.h ${fetch_imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp)
		target_link_libraries (imgui_d3d12_tinywm imgui)

		add_library (imgui_d3d11_tinywm ${fetch_imgui_SOURCE_DIR}/backends/imgui_impl_dx11.h ${fetch_imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp)
		target_link_libraries (imgui_d3d11_tinywm imgui)
	endif ()
	
	if (APPLE)
		add_library (imgui_mtl_tinywm ${fetch_imgui_SOURCE_DIR}/backends/imgui_impl_metal.h ${fetch_imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm)
		target_compile_definitions(imgui_mtl_tinywm PUBLIC IMGUI_IMPL_METAL_CPP)
		target_compile_definitions(imgui_mtl_tinywm PUBLIC IMGUI_IMPL_METAL_CPP_EXTENSIONS)
		target_link_libraries (imgui_mtl_tinywm imgui metalcpp "-framework Carbon" "-framework GameController")
	endif()
	
	message(STATUS "Dear ImGui enabled")
endif ()

# vk-bootstrap
if (HAVE_VULKAN1)
	set (CMAKE_FOLDER 3rdparty/vk-bootstrap)
	FetchContent_Declare(
		fetch_vk_bootstrap
		GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
		GIT_TAG        v1.3.302
		GIT_PROGRESS   TRUE
		GIT_SHALLOW    ON
		UPDATE_COMMAND  ""
	)
	set (CMAKE_MESSAGE_LOG_LEVEL WARNING)
	set (VK_BOOTSTRAP_TEST OFF)
	FetchContent_MakeAvailable(fetch_vk_bootstrap)
	set (CMAKE_MESSAGE_LOG_LEVEL STATUS)
	message(STATUS "vk-bootstrap enabled")
endif ()

# GLM
if (HAVE_VULKAN1 OR HAVE_OPENGL4)
	FetchContent_Declare(
		fetch_glm
		GIT_REPOSITORY  https://github.com/g-truc/glm
		GIT_TAG         1.0.1
		GIT_PROGRESS    TRUE
		GIT_SHALLOW     ON
		UPDATE_COMMAND  ""
	)
	set (CMAKE_FOLDER 3rdparty/GLM)
	set (CMAKE_MESSAGE_LOG_LEVEL WARNING)
	set (GLM_BUILD_TESTS OFF)
	FetchContent_MakeAvailable(fetch_glm)
	set (CMAKE_MESSAGE_LOG_LEVEL STATUS)
	message(STATUS "GLM enabled")
endif ()

# Restore original install() behavior.
set (_DISABLE_INSTALLATION ${_DISABLE_INSTALLATION_OLD})
