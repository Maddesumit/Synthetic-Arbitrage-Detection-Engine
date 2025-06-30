# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/catch2-prefix/src/catch2")
  file(MAKE_DIRECTORY "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/catch2-prefix/src/catch2")
endif()
file(MAKE_DIRECTORY
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/catch2-prefix/src/catch2-build"
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/catch2-prefix"
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/catch2-prefix/tmp"
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/catch2-prefix/src/catch2-stamp"
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/catch2-prefix/src"
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/catch2-prefix/src/catch2-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/catch2-prefix/src/catch2-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/catch2-prefix/src/catch2-stamp${cfgdir}") # cfgdir has leading slash
endif()
