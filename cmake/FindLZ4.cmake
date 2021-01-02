include(FindPackageHandleStandardArgs)

if (NOT LZ4_FOUND)
	find_library(
		LZ4_LIBRARY
		lz4
		PATH_SUFFIXES
		lib
	)

	find_path(
		LZ4_INCLUDE_DIR
		lz4.h
		PATH_SUFFIXES
		include
	)

	add_library(LZ4::LZ4 STATIC IMPORTED)

	set_target_properties(
		LZ4::LZ4
		PROPERTIES
		IMPORTED_LOCATION
		"${LZ4_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${LZ4_INCLUDE_DIR}"
	)

	find_package_handle_standard_args(LZ4 DEFAULT_MSG LZ4_LIBRARY LZ4_INCLUDE_DIR)
endif()
