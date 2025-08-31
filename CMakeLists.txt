cmake_minimum_required(VERSION 3.20)
project("ImguiPlugin" LANGUAGES C CXX VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/rakhook)
add_subdirectory(libs/samp-api)

include_directories(libs/rakhook/lib/minhook/include)
include_directories(libs/rakhook/include)
include_directories(libs/imgui)
include_directories(libs/samp-api/include)

# file(GLOB_RECURSE SAMP "libs/SAMP/*.h")
file(GLOB_RECURSE IMGUI_SOURCE
	"libs/imgui/imgui.cpp"
	"libs/imgui/imgui_demo.cpp"
	"libs/imgui/imgui_draw.cpp"
	"libs/imgui/imgui_tables.cpp"
	"libs/imgui/imgui_widgets.cpp"
	"libs/imgui/backends/imgui_impl_dx9.cpp"
	"libs/imgui/backends/imgui_impl_win32.cpp"
)

# file(GLOB_RECURSE GAME_SOURCE "libs/GAME/src/*.cpp")
# file(GLOB_RECURSE GAME_HEADER "libs/GAME/src/*.h")

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE
	source/dllmain.cpp
    source/Plugin.cpp
	source/Gui.cpp

    ${IMGUI_SOURCE}
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	OUTPUT_NAME ${PROJECT_NAME}
	SUFFIX ".asi"
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${PROJECT_NAME} PUBLIC d3d9 minhook rakhook sampapi)