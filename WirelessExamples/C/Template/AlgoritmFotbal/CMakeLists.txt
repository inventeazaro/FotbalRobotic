project(AlgoritmFotbal)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(Mosquitto)
if (MOSQUITTO_FOUND)
  include_directories(${MOSQUITTO_INCLUDE_DIR})
  target_link_libraries (${PROJECT_NAME} ${MOSQUITTO_LIBRARIES})
endif (MOSQUITTO_FOUND)

target_link_libraries (${PROJECT_NAME} -lpthread)
