cmake_minimum_required (VERSION 2.8.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/config)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Check whether USEMPI and USECUDA are set, if not, set to FALSE.
if(NOT USEMPI)
  set(USEMPI FALSE)
endif()
if(NOT USECUDA)
  set(USECUDA FALSE)
endif()

# Crash on using CUDA and MPI together, not implemented yet.
if(USEMPI AND USECUDA)
  message(FATAL_ERROR "MPI support for CUDA runs is not supported yet")
endif()

# Load system specific settings if not set, force default.cmake.
if(NOT SYST)
  set(SYST default)
endif()
include(${SYST} OPTIONAL RESULT_VARIABLE SYSTINC)

# Trigger fatal error if illegal module is loaded.
if(${SYSTINC} STREQUAL "NOTFOUND")
  message(FATAL_ERROR "Config file config/" ${SYST} ".cmake does not exist.")
endif()

# Set the default build type to RELEASE.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE CACHE STRING
    "Choose the type of build, options are: None Debug Release." FORCE)
else()
  string(TOUPPER ${CMAKE_BUILD_TYPE} TEMP)
  set(CMAKE_BUILD_TYPE ${TEMP} CACHE STRING
    "Choose the type of build, options are: None Debug Release." FORCE)
endif()

# Start the project only after the system specific settings are loaded.
project(microhhc)

# Display status messages for MPI set precompiler flags.
if(USEMPI)
  message(STATUS "MPI: Enabled.")
  # add the necessary precompiler flag
  add_definitions("-DUSEMPI")
else()
  message(STATUS "MPI: Disabled.")
endif()

# Load the CUDA module in case CUDA is enabled and display status message.
if(USECUDA)
  message(STATUS "CUDA: Enabled.")
  find_package(CUDA)
  add_definitions("-DUSECUDA")
else()
  message(STATUS "CUDA: Disabled.")
endif()

# Only set the compiler flags when the cache is created
# to enable editing of the flags in the CMakeCache.txt file.
if(NOT HASCACHE)
  set(CMAKE_C_FLAGS ${USER_C_FLAGS} CACHE STRING
    "Flags used by the C-compiler during all build types." FORCE)
  set(CMAKE_C_FLAGS_DEBUG ${USER_C_FLAGS_DEBUG} CACHE STRING
    "Flags used by the C-compiler during debug builds." FORCE)
  set(CMAKE_C_FLAGS_RELEASE ${USER_C_FLAGS_RELEASE} CACHE STRING
    "Flags used by the C-compiler during release builds." FORCE)
  set(CMAKE_CXX_FLAGS ${USER_CXX_FLAGS} CACHE STRING
    "Flags used by the CXX-compiler during all build types." FORCE)
  set(CMAKE_CXX_FLAGS_DEBUG ${USER_CXX_FLAGS_DEBUG} CACHE STRING
    "Flags used by the CXX-compiler during debug builds." FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE ${USER_CXX_FLAGS_RELEASE} CACHE STRING
    "Flags used by the CXX-compiler during release builds." FORCE)
  if(USECUDA)
    set(CUDA_NVCC_FLAGS ${USER_CUDA_NVCC_FLAGS} CACHE STRING
      "Flags used by NVCC." FORCE)
  endif()

  message(STATUS "Build Type: " ${CMAKE_BUILD_TYPE})
  set(HASCACHE TRUE CACHE BOOL "CMakeCache.txt created." FORCE)
  # Make sure that ccmake only contains build type.
  mark_as_advanced(HASCACHE)
  mark_as_advanced(CMAKE_INSTALL_PREFIX)
endif()

# Print the C++ and CUDA compiler flags to the screen.
if(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
  message(STATUS "Compiler flags: " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_RELEASE})
else()
  message(STATUS "Compiler flags: " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_DEBUG})
endif()

if(USECUDA)
  # Add CUDA debug definition, to enforce CUDA API and kernel checks.
  if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    add_definitions("-DCUDACHECKS")
  endif()
  message(STATUS "NVCC flags: " ${CUDA_NVCC_FLAGS})
endif()

add_subdirectory(examples)
