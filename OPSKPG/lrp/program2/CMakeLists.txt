cmake_minimum_required(VERSION 3.5.0)
project(program2 VERSION 0.1.0 LANGUAGES C CXX)

find_library(NTL_LIB ntl)
if(NOT NTL_LIB)
    message(FATAL_ERROR "not")
endif()

add_executable(program2 main.cpp )
target_link_libraries(program2 ${NTL_LIB} pthread)


