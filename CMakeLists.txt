cmake_minimum_required(VERSION 3.10)
project(SpaceInvaders)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Add the executable
add_executable(main main.cpp)

# Find required packages
find_package(OpenGL REQUIRED)

# Link GLFW and GLEW (assumes they are installed and findable via pkg-config or system path)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)
message(STATUS "GLEW_INCLUDE_DIRS: ${GLEW_INCLUDE_DIRS}")
message(STATUS "GLEW_LIBRARIES: ${GLEW_LIBRARIES}")
message(STATUS "GLFW_INCLUDE_DIRS: ${GLFW_INCLUDE_DIRS}")
message(STATUS "GLFW_LIBRARIES: ${GLFW_LIBRARIES}")
get_target_property(LINK_LIBS main LINK_LIBRARIES)
message(STATUS "main is linking with: ${LINK_LIBS}")
# Include directories
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})

# Link libraries
target_link_libraries(main
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
)

# macOS specific: link to OpenGL framework
if(APPLE)
    target_link_libraries(main "-framework OpenGL")
endif()

