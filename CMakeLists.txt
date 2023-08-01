cmake_minimum_required(VERSION 3.20)
project(tennis_triple_love)

set(CMAKE_CXX_STANDARD 20)

IF (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
ENDIF()

# finds our own OpenGL dependency from installed binaries
find_package(OpenGL REQUIRED)

# includes vendors' source CMake projects
add_subdirectory(vendor)

# add s_engine project and link glfw to it
file(GLOB_RECURSE TENNIS_TRIPLE_LOVE_FILES source/**.cpp)
add_executable(tennis_triple_love ${TENNIS_TRIPLE_LOVE_FILES})

target_include_directories(tennis_triple_love PRIVATE source)

IF (WIN32)
    target_link_libraries(tennis_triple_love PRIVATE glfw OpenGL::GL glm glad -static-libgcc -static-libstdc++)
ELSE()
    target_link_libraries(tennis_triple_love PRIVATE glfw OpenGL::GL glm glad)
ENDIF()