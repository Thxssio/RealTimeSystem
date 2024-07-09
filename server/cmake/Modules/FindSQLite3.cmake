# FindSQLite3.cmake
find_path(SQLite3_INCLUDE_DIR NAMES sqlite3.h)
find_library(SQLite3_LIBRARY NAMES sqlite3)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3 DEFAULT_MSG SQLite3_LIBRARY SQLite3_INCLUDE_DIR)

mark_as_advanced(SQLite3_INCLUDE_DIR SQLite3_LIBRARY)

if(SQLite3_FOUND)
    set(SQLite3_LIBRARIES ${SQLite3_LIBRARY})
    set(SQLite3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIR})
endif()
