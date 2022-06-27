cmake_minimum_required(VERSION 3.5)

project(pseolang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# pseolang
set(pseolang_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(pseolang_INCLUDE_DIR "${pseolang_DIR}/include")
set(pseolang_SOURCE_DIR "${pseolang_DIR}/src")
set(pseolang_LIBRARY_DIR "${pseolang_DIR}/lib")

file(GLOB_RECURSE pseolang_INCLUDE_FILES "${pseolang_INCLUDE_DIR}/*")
file(GLOB_RECURSE pseolang_SOURCE_FILES "${pseolang_SOURCE_DIR}/*")

# Create the static library
add_library(${PROJECT_NAME}
	STATIC
	${PSEOLANG_INCLUDE_FILES} ${PSEOLANG_SOURCE_FILES}
)

# Place the library in the current project directory
set_target_properties(${PROJECT_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${PSEOLANG_LIBRARY_DIR} # Static library
	LIBRARY_OUTPUT_DIRECTORY ${PSEOLANG_LIBRARY_DIR} # Dynamic library
)

# Set include directories to use when compiling
target_include_directories(${PROJECT_NAME}
	PUBLIC
	${PSEOLANG_INCLUDE_DIR}
)
