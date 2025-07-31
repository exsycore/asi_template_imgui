cmake_minimum_required(VERSION 3.20)
project("SF-DIALOG" LANGUAGES C CXX VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/MinHook)
add_subdirectory(libs/RakHook)

include_directories(libs/MinHook/include)
include_directories(libs/RakHook/include)
include_directories(libs/SAMP)
include_directories(libs/Imgui)

# file(GLOB_RECURSE SAMP "libs/SAMP/*.h")
file(GLOB_RECURSE IMGUI_SOURCE
	"libs/Imgui/imgui.cpp"
	"libs/Imgui/imgui_demo.cpp"
	"libs/Imgui/imgui_draw.cpp"
	"libs/Imgui/imgui_tables.cpp"
	"libs/Imgui/imgui_widgets.cpp"
	"libs/Imgui/backends/imgui_impl_dx9.cpp"
	"libs/Imgui/backends/imgui_impl_win32.cpp"
)

# file(GLOB_RECURSE GAME_SOURCE "libs/GAME/src/*.cpp")
# file(GLOB_RECURSE GAME_HEADER "libs/GAME/src/*.h")

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE
	source/dllmain.cpp
    source/Plugin.cpp

    ${IMGUI_SOURCE}
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	OUTPUT_NAME ${PROJECT_NAME}
	SUFFIX ".asi"
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${PROJECT_NAME} PUBLIC d3d9 cyanide::cyanide minhook rakhook)