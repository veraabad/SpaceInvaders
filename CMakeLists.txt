set(APP_NAME SpaceInvaders)
cmake_minimum_required(VERSION 3.10)
project(OpenGLApp)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)

include_directories(${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} "src/include")
link_directories(${GLEW_LIBRARY_DIRS} ${GLFW_LIBRARY_DIRS})

# Find actual libs
find_library(GLEW_ACTUAL_LIB NAMES GLEW glew PATHS ${GLEW_LIBRARY_DIRS} /usr/local/lib)
find_library(GLFW_ACTUAL_LIB NAMES glfw PATHS ${GLFW_LIBRARY_DIRS} /usr/local/lib)

message(STATUS "Resolved GLEW to: ${GLEW_ACTUAL_LIB}")
message(STATUS "Resolved GLFW to: ${GLFW_ACTUAL_LIB}")

add_executable(${APP_NAME} 
    src/main.cpp 
    src/utility.cpp
    src/sprites.cpp
)

target_link_libraries(${APP_NAME}
    ${GLEW_ACTUAL_LIB}
    ${GLFW_ACTUAL_LIB}
    "-framework OpenGL"
)
