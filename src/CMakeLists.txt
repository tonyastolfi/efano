cmake_minimum_required(VERSION 3.20)

cxx_library(efano)

target_link_libraries(
  efano
  PUBLIC boost::boost
  #
  # Add other library -> library dependencies here.
  #
  xxHash::xxhash
  batteries::batteries
  )

target_link_libraries(
  efano_Test
  PRIVATE gtest::gtest
  #
  # Add other test -> library dependencies here
  #
  xxHash::xxhash
  batteries::batteries
  )
