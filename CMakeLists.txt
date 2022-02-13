cmake_minimum_required(VERSION 3.13)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif()

project(
    "Piupiu"        
    VERSION 0.1.0  
    LANGUAGES C CXX
)

option(USE_GUI "Build a graphical client" OFF)

if(NOT USE_GUI)
    message(STATUS "Compilando sem interface gráfica. Use a opção -DUSE_GUI=ON para compilar com interface gráfica.")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Custom modules and scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake") # Make our cmake scripts available

# include(LTO)
include(Warnings)
# include(CopyDllsForDebug)
# include(Coverage)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(external)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(source/server)
add_subdirectory(source/client)

