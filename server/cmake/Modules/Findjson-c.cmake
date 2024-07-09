# Findjson-c.cmake
find_path(JSON-C_INCLUDE_DIR NAMES json.h PATH_SUFFIXES json-c)
find_library(JSON-C_LIBRARY NAMES json-c)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(json-c DEFAULT_MSG JSON-C_LIBRARY JSON-C_INCLUDE_DIR)

if(JSON-C_FOUND)
    set(JSON-C_LIBRARIES ${JSON-C_LIBRARY})
    set(JSON-C_INCLUDE_DIRS ${JSON-C_INCLUDE_DIR})
endif()
