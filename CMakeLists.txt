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

add_executable(render3d ${SRC_FILES})

set_target_properties(render3d PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
# Include directory (IMPORTANT)
target_include_directories(render3d PRIVATE include)
target_link_libraries(render3d ${SDL2_LIBRARIES})
