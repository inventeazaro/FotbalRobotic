project(TrackingServer)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

add_executable(${PROJECT_NAME} ${SRC_LIST})

FIND_PACKAGE(OpenCV REQUIRED core highgui imgproc)

set(REQUIRED_LIBRARIES ${OpenCV_LIBS})
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
