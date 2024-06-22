# - Try to find microhttpd
# Once done this will define
#  MICROHTTPD_FOUND - System has microhttpd
#  MICROHTTPD_INCLUDE_DIRS - The microhttpd include directories
#  MICROHTTPD_LIBRARIES - The libraries needed to use microhttpd

find_path(MICROHTTPD_INCLUDE_DIR microhttpd.h)
find_library(MICROHTTPD_LIBRARY NAMES microhttpd libmicrohttpd)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Microhttpd DEFAULT_MSG
                                  MICROHTTPD_INCLUDE_DIR
                                  MICROHTTPD_LIBRARY)

if(MICROHTTPD_FOUND)
  set(MICROHTTPD_LIBRARIES ${MICROHTTPD_LIBRARY})
  set(MICROHTTPD_INCLUDE_DIRS ${MICROHTTPD_INCLUDE_DIR})
else()
  set(MICROHTTPD_LIBRARIES)
  set(MICROHTTPD_INCLUDE_DIRS)
endif()
