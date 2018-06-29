cmake_minimum_required(VERSION 3.5)
project(argus_camera)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)

find_package(TegraMM REQUIRED)
if(${TegraMM_FOUND})
  message(STATUS "Found Tegra Multimedia API")
else()
  message(STATUS "Could not find Tegra Multimedia API")
endif()

add_library(argus_camera SHARED
  src/ArgusCamera.h
  src/ArgusCamera.cpp
)

target_link_libraries(argus_camera TegraMM ${TegraMM_LIBRARIES})

install(TARGETS argus_camera
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static)
