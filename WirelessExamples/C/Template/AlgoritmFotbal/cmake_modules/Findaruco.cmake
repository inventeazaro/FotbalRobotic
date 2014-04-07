# - Find aruco
# Find the native aruco includes and libraries
#
#  ARUCO_INCLUDE_DIR - where to find mosquitto.h, etc.
#  ARUCO_LIBRARIES   - List of libraries when using libmosquitto.
#  ARUCO_FOUND       - True if libmosquitto found.

if(ARUCO_INCLUDE_DIR)
    # Already in cache, be silent
    set(ARUCO_FIND_QUIETLY TRUE)
endif(ARUCO_INCLUDE_DIR)

find_path(ARUCO_INCLUDE_DIR
          aruco.h
          PATHS /usr/local/include/aruco)

find_library(ARUCO_LIBRARY
             PATHS /usr/local/lib
             NAMES libaruco aruco)

# Handle the QUIETLY and REQUIRED arguments and set ARUCO_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ARUCO DEFAULT_MSG ARUCO_LIBRARY ARUCO_INCLUDE_DIR)

if(ARUCO_FOUND)
  set(ARUCO_LIBRARIES ${ARUCO_LIBRARY})
else(ARUCO_FOUND)
  set(ARUCO_LIBRARIES)
endif(ARUCO_FOUND)

mark_as_advanced(ARUCO_INCLUDE_DIR ARUCO_LIBRARY)
