# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/spdlog-prefix/src/spdlog")
  file(MAKE_DIRECTORY "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/spdlog-prefix/src/spdlog")
endif()
file(MAKE_DIRECTORY
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/spdlog-prefix/src/spdlog-build"
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/spdlog-prefix"
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/spdlog-prefix/tmp"
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/spdlog-prefix/src/spdlog-stamp"
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/spdlog-prefix/src"
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/spdlog-prefix/src/spdlog-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/spdlog-prefix/src/spdlog-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/spdlog-prefix/src/spdlog-stamp${cfgdir}") # cfgdir has leading slash
endif()
