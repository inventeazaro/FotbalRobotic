project(TrackingServer)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

add_executable(${PROJECT_NAME} ${SRC_LIST})

option(USE_STATIC_BOOST "Build with static BOOST libraries instead of dynamic" NO)
set(Boost_USE_MULTITHREADED OFF)
unset(Boost_INCLUDE_DIR CACHE)
unset(Boost_LIBRARY_DIRS CACHE)
set(BOOST_ROOT "/usr/lib/x86_64-linux-gnu" CACHE PATH "")

if(USE_STATIC_BOOST)
   unset(Boost_LIBRARIES)
   message(WARNING "Linking against boost static libraries")
   set(Boost_USE_STATIC_LIBS ON)
   find_package(Boost REQUIRED COMPONENTS regex program_options system)
else(USE_STATIC_BOOST)
   unset(Boost_LIBRARIES)
   message(WARNING "Linking against boost dynamic libraries")
   set(Boost_USE_STATIC_LIBS OFF)
   find_package(Boost REQUIRED COMPONENTS regex program_options system)
endif(USE_STATIC_BOOST)

FIND_PACKAGE(OpenCV REQUIRED core highgui imgproc)

set(REQUIRED_LIBRARIES ${OpenCV_LIBS})

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${SYSLOGPP_LIBRARY})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBS})

FIND_PACKAGE(Mosquitto)
if (MOSQUITTO_FOUND)
  include_directories(${MOSQUITTO_INCLUDE_DIR})
  target_link_libraries (${PROJECT_NAME} ${MOSQUITTO_LIBRARIES})
endif (MOSQUITTO_FOUND)

FIND_PACKAGE(OpenSSL)
if(OPENSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIRS})
  target_link_libraries (${PROJECT_NAME} ${OPENSSL_LIBRARIES})
endif(OPENSSL_FOUND)

FIND_PACKAGE(aruco)
if(ARUCO_FOUND)
  include_directories(${ARUCO_INCLUDE_DIRS})
  target_link_libraries (${PROJECT_NAME} ${ARUCO_LIBRARIES})
endif(ARUCO_FOUND)

target_link_libraries (${PROJECT_NAME} -lpthread)
