# CMakeList.txt: CMake_linux_http_server_formal 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED	ON)
set(CMAKE_CXX_EXTENSIONS ON)

project(CMake_linux_http_server_formal LANGUAGES C CXX)

find_package(Threads)
# 将源代码添加到此项目的可执行文件。

add_executable (CMake_linux_http_server_formal)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)
target_sources(CMake_linux_http_server_formal PUBLIC ${sources})
if (CMAKE_VERSION VERSION_GREATER 3.24)
  set_property(TARGET CMake_linux_http_server_formal PROPERTY CXX_STANDARD 20)
  
endif()

# TODO: 如有需要，请添加测试并安装目标。
