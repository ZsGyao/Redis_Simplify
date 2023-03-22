cmake_minimum_required(VERSION 3.22)
project(Redis_Simplify)

set(CMAKE_C_STANDARD 11)

include (cmake/utils.cmake)

# -rdynamic: 将所有符号都加入到符号表中，便于使用dlopen或者backtrace追踪到符号
# -fPIC: 生成位置无关的代码，便于动态链接
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -fPIC")

# 指定编译选项
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -std=c++11 -O0 -ggdb -Wall -Werror")

# -Wno-unused-function: 不要警告未使用函数
# -Wno-builtin-macro-redefined: 不要警告内置宏重定义，用于重定义内置的__FILE__宏
# -Wno-deprecated: 不要警告过时的特性
# -Wno-deprecated-declarations: 不要警告使用带deprecated属性的变量，类型，函数
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated -Wno-deprecated-declarations")


include_directories(.)
include_directories(src)

option(BUILD_TEST "ON for complile test" ON)

set(LIB_SRC
        src/adlist.cpp
        )

set(LIBS
        src
        )

add_library(src SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(src)

add_executable(test_adlist tests/test_adlist.cpp)
add_dependencies(test_adlist src)
force_redefine_file_macro_for_sources(test_adlist)  #__FILE__
target_link_libraries(test_adlist ${LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)