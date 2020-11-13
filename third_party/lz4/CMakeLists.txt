include(ExternalProject)

ExternalProject_Add(
  lz4
  URL           "https://github.com/lz4/lz4/archive/dev.zip"
  SOURCE_SUBDIR "build/cmake"
  INSTALL_DIR		"${CMAKE_BINARY_DIR}/vendor"
  PATCH_COMMAND	"${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third_party/lz4/CMakeLists.txt.in" "<SOURCE_DIR>/build/cmake/CMakeLists.txt"
  CMAKE_ARGS
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_PREFIX_PATH=<INSTALL_DIR>"
    "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
    "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}"
    "-DLZ4_BUILD_CLI=OFF"
    "-DLZ4_BUILD_LEGACY_LZ4C=OFF"
)
