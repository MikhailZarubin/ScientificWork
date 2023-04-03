cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME GlobalOptimization)
project(${PROJECT_NAME})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GCGen/src/)

set(SOURCE_EXE main.cpp)
file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
	            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	            ${CMAKE_CURRENT_SOURCE_DIR}/GCGen/sample_src/Grishagin/*.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/GCGen/sample_src/Grishagin/*.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/GCGen/src/*.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/GCGen/src/*.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/GCGen/src/*.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_EXE} ${SRC_FILES})