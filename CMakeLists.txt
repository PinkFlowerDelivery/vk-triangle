cmake_minimum_required(VERSION 3.23)

project(vkTriangle)

add_executable(vkTriangle src/main.cpp src/vkTreang.cpp src/checkSuitableUtils.cpp)

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        12.1.0)
FetchContent_MakeAvailable(fmt)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(vkTriangle fmt::fmt Vulkan::Vulkan glfw)
