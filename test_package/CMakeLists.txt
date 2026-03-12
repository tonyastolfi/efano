cmake_minimum_required(VERSION 3.20)
project(PackageTest CXX)

find_package(efano CONFIG REQUIRED)


add_executable(example src/example.cpp)
target_link_libraries(example efano::efano)
