cmake_minimum_required(VERSION 3.5)

project(PRegEx)

set(BUILD_SHARED_LIBS ON)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-Xlinker -zmuldefs -Wall -p -pg -g -pedantic")

# Release flags
set(CMAKE_CXX_FLAGS_RELEASE "-Xlinker -zmuldefs -Wall -O3")

# Source files
set(pregexlib_SRC src/pregex.cpp)
set(pregexbin_SRC src/pregex.cpp src/parser.cpp)

# Library and executable targets
add_library(PRegEx ${pregexlib_SRC})
add_executable(parser ${pregexbin_SRC})

# Installation
SET(CMAKE_INSTALL_PREFIX /usr)
install(TARGETS PRegEx DESTINATION lib)
install(DIRECTORY DESTINATION include/pregex)
install(FILES src/pregex.h DESTINATION include/pregex)
