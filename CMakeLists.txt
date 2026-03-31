cmake_minimum_required(VERSION 3.10)

project(Render3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL2
find_package(SDL2 REQUIRED)

# Source files
file(GLOB SRC_FILES
    src/*.cpp
)
# ImGui source files (explicit is better than GLOB for libs)
set(IMGUI_SOURCES
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/backends/imgui_impl_sdl2.cpp
    libs/imgui/backends/imgui_impl_sdlrenderer2.cpp
)
add_executable(render3d ${SRC_FILES} ${IMGUI_SOURCES})

set_target_properties(render3d PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
# Include directory (IMPORTANT)
target_include_directories(render3d PRIVATE 
  include
  libs/imgui
  libs/imgui/backends
  ${SDL2_INCLUDE_DIRS}
)
target_link_libraries(render3d ${SDL2_LIBRARIES})
