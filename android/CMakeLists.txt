project(nitrostate)
cmake_minimum_required(VERSION 3.9.0)

set(PACKAGE_NAME nitrostate)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

# C++ source files
set(CPP_SOURCES
    ../cpp/AtomCore.cpp
    ../cpp/ComputedCore.cpp
    ../cpp/BatchManager.cpp
    ../cpp/HybridNitroState.cpp
)

# Define C++ library and add all sources
add_library(${PACKAGE_NAME} SHARED
    src/main/cpp/cpp-adapter.cpp
    ${CPP_SOURCES}
)

# Add Nitrogen specs
include(${CMAKE_SOURCE_DIR}/../nitrogen/generated/android/nitrostate+autolinking.cmake)

# Set up local includes
include_directories(
    "src/main/cpp"
    "../cpp"
)

find_library(LOG_LIB log)

# Link all libraries together
target_link_libraries(
    ${PACKAGE_NAME}
    ${LOG_LIB}
    android
)
