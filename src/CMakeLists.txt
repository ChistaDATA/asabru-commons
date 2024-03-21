cmake_minimum_required(VERSION 3.15)
project(asabru_commons VERSION 1.0)
# Enables generation of compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Add compiler warnings
if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-Wall)
endif ()

# Set DEBUG preprocessor flag
add_compile_definitions(DEBUG)

# Find OpenSSL library
find_package(OpenSSL REQUIRED)

# Find Python
find_package(Python COMPONENTS Interpreter Development)
if (Python_FOUND)
    message(STATUS "Python found: ${Python_EXECUTABLE}")
else()
    message(FATAL_ERROR "Python not found.")
endif()
include_directories(${Python_INCLUDE_DIRS})

# TinyXML
include_directories(../../tinyxml2)
include_directories(../../stduuid/include)

# Find SqlLite3
find_package (SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

# Add jwt-cpp library
include_directories(../../jwt-cpp/include)

# Set source directory
set(SOURCE_DIRECTORY ".")

# Collect all source files
file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIRECTORY}/*.cpp" "${SOURCE_DIRECTORY}/**/*.cpp")

# Add build directory
set(BUILD_DIRECTORY "../build")

# Set library directory
set(LIB_DIRECTORY "lib")

# Create a library target
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

# Set output directory for libraries
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIB_DIRECTORY}
        )

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC OpenSSL::SSL
        PUBLIC ${SQLite3_LIBRARIES}
        PUBLIC ${Python_LIBRARIES}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../tinyxml2/build/libtinyxml2.a
)


