cmake_minimum_required(VERSION 2.8)
project(opym CXX)
enable_language(CXX C)
include(CTest)
include(TestCXXAcceptsFlag)

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -pedantic ${CMAKE_CXX_FLAGS}")

find_package(opm-parser REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)
find_package(Boost COMPONENTS python REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIR}
                    ${opm-parser_INCLUDE_DIRS})


add_library(opym SHARED src/opym.cpp)
target_link_libraries(opym ${Boost_LIBRARIES} ${opm-parser_LIBRARIES})
set_target_properties(opym PROPERTIES PREFIX "")
