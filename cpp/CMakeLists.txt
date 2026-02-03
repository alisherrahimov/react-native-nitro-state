cmake_minimum_required(VERSION 3.13)
project(nitrostate)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    AtomCore.cpp
    ComputedCore.cpp
    BatchManager.cpp
    HybridNitroState.cpp
)

# Header files
set(HEADERS
    AtomCore.hpp
    ComputedCore.hpp
    BatchManager.hpp
    HybridNitroState.hpp
)

# Create library
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link with React Native and JSI
find_package(ReactAndroid REQUIRED CONFIG)
find_package(fbjni REQUIRED CONFIG)

target_link_libraries(${PROJECT_NAME}
    ReactAndroid::jsi
    ReactAndroid::react_nativemodule_core
    fbjni::fbjni
)

# Nitro Modules
find_package(NitroModules REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME}
    NitroModules::NitroModules
)
