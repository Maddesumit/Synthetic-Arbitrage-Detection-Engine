# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

if(EXISTS "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd-stamp/xsimd-gitclone-lastrun.txt" AND EXISTS "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd-stamp/xsimd-gitinfo.txt" AND
  "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd-stamp/xsimd-gitclone-lastrun.txt" IS_NEWER_THAN "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd-stamp/xsimd-gitinfo.txt")
  message(VERBOSE
    "Avoiding repeated git clone, stamp file is up to date: "
    "'/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd-stamp/xsimd-gitclone-lastrun.txt'"
  )
  return()
endif()

# Even at VERBOSE level, we don't want to see the commands executed, but
# enabling them to be shown for DEBUG may be useful to help diagnose problems.
cmake_language(GET_MESSAGE_LOG_LEVEL active_log_level)
if(active_log_level MATCHES "DEBUG|TRACE")
  set(maybe_show_command COMMAND_ECHO STDOUT)
else()
  set(maybe_show_command "")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd"
  RESULT_VARIABLE error_code
  ${maybe_show_command}
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/opt/homebrew/bin/git"
            clone --no-checkout --config "advice.detachedHead=false" "https://github.com/xtensor-stack/xsimd.git" "xsimd"
    WORKING_DIRECTORY "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src"
    RESULT_VARIABLE error_code
    ${maybe_show_command}
  )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(NOTICE "Had to git clone more than once: ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/xtensor-stack/xsimd.git'")
endif()

execute_process(
  COMMAND "/opt/homebrew/bin/git"
          checkout "10.0.0" --
  WORKING_DIRECTORY "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd"
  RESULT_VARIABLE error_code
  ${maybe_show_command}
)
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: '10.0.0'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/opt/homebrew/bin/git" 
            submodule update --recursive --init 
    WORKING_DIRECTORY "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd"
    RESULT_VARIABLE error_code
    ${maybe_show_command}
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd-stamp/xsimd-gitinfo.txt" "/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd-stamp/xsimd-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  ${maybe_show_command}
)
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/Users/sumitmadde/Desktop/Synthetic Pair Deviation Engine/build/xsimd-prefix/src/xsimd-stamp/xsimd-gitclone-lastrun.txt'")
endif()
