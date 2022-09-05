# CMakeList.txt: CMake_linux_http_server_formal 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("CMake_linux_http_server_formal")
find_package(Threads)
# 将源代码添加到此项目的可执行文件。

add_executable (CMake_linux_http_server_formal "CMake_linux_http_server_formal.cpp" "CMake_linux_http_server_formal.h" "ThreadPool.h" "ThreadPool.cpp" "select.cpp"  "select.h" "httpfunction.cpp" "httpfunction.h")
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMake_linux_http_server_formal PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
