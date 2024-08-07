#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LZ4::lz4_shared" for configuration "Release"
set_property(TARGET LZ4::lz4_shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LZ4::lz4_shared PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/lz4.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/lz4.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS LZ4::lz4_shared )
list(APPEND _IMPORT_CHECK_FILES_FOR_LZ4::lz4_shared "${_IMPORT_PREFIX}/lib/lz4.lib" "${_IMPORT_PREFIX}/bin/lz4.dll" )

# Import target "LZ4::lz4_static" for configuration "Release"
set_property(TARGET LZ4::lz4_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LZ4::lz4_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/lz4_static.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS LZ4::lz4_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_LZ4::lz4_static "${_IMPORT_PREFIX}/lib/lz4_static.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
