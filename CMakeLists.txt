cmake_minimum_required(VERSION 3.20)

project(PRegEx)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compiler flags depending on the compiler
if(MSVC)
    # Debug flags for MSVC
    set(CMAKE_CXX_FLAGS_DEBUG "/W3 /Zi /Od /MDd")
    
    # Release flags for MSVC
    set(CMAKE_CXX_FLAGS_RELEASE "/W3 /O2 /MD")
else()
    # Debug flags for Unix-like systems (e.g., GCC/Clang)
    set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -pedantic")

    # Release flags for Unix-like systems
    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3")
endif()

# Source files
set(pregexlib_SRC src/pregex.cpp)
set(pregexbin_SRC src/pregex.cpp src/parser.cpp)

# Library and executable targets
add_library(PRegEx ${pregexlib_SRC})
add_executable(parser ${pregexbin_SRC})

# Installation
if(UNIX)
    SET(CMAKE_INSTALL_PREFIX /usr)
    install(TARGETS PRegEx DESTINATION lib)
    install(DIRECTORY DESTINATION include/pregex)
    install(FILES src/pregex.h DESTINATION include/pregex)
else()
    # Windows installation settings (can be adjusted based on your project needs)
    SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
    install(TARGETS PRegEx DESTINATION bin)
    install(FILES src/pregex.h DESTINATION include/pregex)
endif()
