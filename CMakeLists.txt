cmake_minimum_required(VERSION 3.5)

project(pseolang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# pseolang
set(PSEOLANG_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(PSEOLANG_INCLUDE_DIR "${PSEOLANG_DIR}/include")
set(PSEOLANG_SOURCE_DIR "${PSEOLANG_DIR}/src")
set(PSEOLANG_LIBRARY_DIR "${PSEOLANG_DIR}/lib")

file(GLOB_RECURSE PSEOLANG_INCLUDE_FILES "${PSEOLANG_INCLUDE_DIR}/*")
file(GLOB_RECURSE PSEOLANG_SOURCE_FILES "${PSEOLANG_SOURCE_DIR}/*")

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
	PRIVATE
	${PSEOLANG_INCLUDE_DIR}
)
