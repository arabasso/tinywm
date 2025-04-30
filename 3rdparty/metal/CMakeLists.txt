add_library(metalcpp metal.cpp)
target_include_directories(metalcpp PUBLIC "metal-cpp" "metal-cpp-extensions")
target_link_libraries(metalcpp "-framework Metal" "-framework MetalKit" "-framework AppKit" "-framework Foundation" "-framework QuartzCore")