# pseolang
set(pseolang_FOUND ON)

set(pseolang_DIR ${CMAKE_CURRENT_LIST_DIR})

set(pseolang_INCLUDE_DIR "${pseolang_DIR}/include")
set(pseolang_INCLUDE_DIRS ${pseolang_INCLUDE_DIR})
set(pseolang_LIBRARY_DIR "${pseolang_DIR}/lib")

file(GLOB_RECURSE pseolang_LIBRARIES "${pseolang_LIBRARY_DIR}/*")

# Total files in the libraries folder
list(LENGTH pseolang_LIBRARIES pseolang_LIBRARIES_LEN)

# When run without a compiled library,
# it gets installed
if(pseolang_LIBRARIES_LEN EQUAL 0)
	add_subdirectory(${pseolang_DIR})
else()
	# Create the static library
	add_library(pseolang STATIC IMPORTED)

	# Set files to use when compiling
	set_property(TARGET pseolang
		PROPERTY
		INTERFACE_INCLUDE_DIRECTORIES ${pseolang_INCLUDE_DIRS}
	)

	set_property(TARGET pseolang
		PROPERTY
		IMPORTED_LOCATION ${pseolang_LIBRARIES}
	)
endif()
