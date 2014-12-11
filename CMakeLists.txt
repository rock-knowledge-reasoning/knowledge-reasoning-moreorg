# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 2.6)
find_package(Rock)
rock_init(owl_om 0.1)
list(APPEND CMAKE_CXX_FLAGS "-std=c++11")
rock_standard_layout()
