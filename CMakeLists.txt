# Top-Level CMakeList.txt
cmake_minimum_required(VERSION 2.6)
project(Lab)

# ---- FLAGS ----
# General
set(CMAKE_CXX_STANDARD 14)                       # Use C++ 14.
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
# Debug
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address -fno-omit-frame-pointer")
# Release
set(CMAKE_CXX_FLAGS_RELEASE -O3)


add_subdirectory(libraries/SDL2-2.0.9)



# ---- LABS -----
# Lab1
add_executable(Lab1 source/lab1.cpp)
target_link_libraries(Lab1 SDL2)
target_include_directories(Lab1 PRIVATE libraries/glm/)
target_include_directories(Lab1 PRIVATE includes/)


# Lab2
add_executable(Lab2 source/lab2.cpp)
target_link_libraries(Lab2 SDL2)
target_include_directories(Lab2 PRIVATE libraries/glm/)
target_include_directories(Lab2 PRIVATE includes/)


# Lab3
add_executable(Lab3 source/lab3.cpp)
target_link_libraries(Lab3 SDL2)
target_include_directories(Lab3 PRIVATE libraries/glm/)
target_include_directories(Lab3 PRIVATE includes/)


# ---- TESTS ----

# Interpolation
add_executable(TestInterpolation tests/interpolation.cpp)
target_include_directories(TestInterpolation PRIVATE libraries/glm/)
target_include_directories(TestInterpolation PRIVATE libraries/test)
target_include_directories(TestInterpolation PRIVATE includes/)

# Intersection
add_executable(TestIntersection tests/intersection.cpp)
target_include_directories(TestIntersection PRIVATE libraries/test)
target_include_directories(TestIntersection PRIVATE libraries/glm/)
target_include_directories(TestIntersection PRIVATE includes/)

# SDL2
add_executable(TestSDL2 tests/SDL2.cpp)
target_link_libraries(TestSDL2 SDL2)
target_include_directories(TestSDL2 PRIVATE libraries/test)
target_include_directories(TestSDL2 PRIVATE libraries/glm/)
target_include_directories(TestSDL2 PRIVATE includes/)

# SDL2
add_executable(TestCamera tests/Camera.cpp)
target_include_directories(TestCamera PRIVATE libraries/test)
target_include_directories(TestCamera PRIVATE libraries/glm/)
target_include_directories(TestCamera PRIVATE includes/)


# ---- OTHERS ----
# Skeleton
add_executable(Skeleton source/skeleton.cpp)
target_link_libraries(Skeleton SDL2)
target_include_directories(Skeleton PRIVATE libraries/glm/)
target_include_directories(Skeleton PRIVATE includes/)

# Window
add_executable(Window source/window.cpp)
target_include_directories(Window PRIVATE /opt/X11/include)