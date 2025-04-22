cmake_minimum_required(VERSION 3.10)
project(SpaceInvaders)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Add the executable
add_executable(main main.cpp)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)

find_library(GLEW_ACTUAL_LIB NAMES GLEW glew PATHS ${GLEW_LIBRARY_DIRS})
find_library(GLFW_ACTUAL_LIB NAMES glfw PATHS ${GLFW_LIBRARY_DIRS})

message(STATUS "Resolved GLEW to: ${GLEW_ACTUAL_LIB}")
message(STATUS "Resolved GLFW to: ${GLFW_ACTUAL_LIB}")
# Print debugging info
message(STATUS "GLEW_INCLUDE_DIRS: ${GLEW_INCLUDE_DIRS}")
message(STATUS "GLEW_LIBRARY_DIRS: ${GLEW_LIBRARY_DIRS}")
message(STATUS "GLEW_LIBRARIES: ${GLEW_LIBRARIES}")
message(STATUS "GLFW_INCLUDE_DIRS: ${GLFW_INCLUDE_DIRS}")
message(STATUS "GLFW_LIBRARY_DIRS: ${GLFW_LIBRARY_DIRS}")
message(STATUS "GLFW_LIBRARIES: ${GLFW_LIBRARIES}")

# Include directories from pkg-config
include_directories(${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS} ${GLFW_LIBRARY_DIRS})

# Link to actual libraries and macOS framework
target_link_libraries(main
    ${GLEW_ACTUAL_LIB}
    ${GLFW_ACTUAL_LIB}
    "-framework OpenGL"
)

# Double-check what's being linked
get_target_property(LINK_LIBS main LINK_LIBRARIES)
message(STATUS "main is linking with: ${LINK_LIBS}")
