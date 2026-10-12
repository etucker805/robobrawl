#######################################################################
#
cmake_minimum_required(VERSION 3.28)
project (robobrawl_control)


# get the CoreDX DDS config:
include( ${CMAKE_CURRENT_LIST_DIR}/../cmake/coredx.cmake )
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")

# build the TMS core libraries:
# if( NOT DEFINED ROBOBRAWL_INTERFACE_BUILD)
  # add_subdirectory( ${CMAKE_CURRENT_LIST_DIR}/../robobrawl ./robobrawl )
# endif()

#######################################################################
#
# Here is an example application that implements a STORAGE device
# and utilizes the tms_interface library for the TMS implementation.
#
add_executable(clock_app
  )


target_sources(clock_app PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/main.cc
  ${CMAKE_CURRENT_LIST_DIR}/init.cpp
  ${CMAKE_CURRENT_LIST_DIR}/cleanup.cpp
  ${CMAKE_CURRENT_LIST_DIR}/main_loop.cpp
  ${CMAKE_CURRENT_LIST_DIR}/startup.cpp
  # ${CMAKE_CURRENT_LIST_DIR}/interface/clock_driver.cpp
  # ${CMAKE_CURRENT_LIST_DIR}/interface/state_tree.cpp
  ${CMAKE_CURRENT_LIST_DIR}/roles/clock.cpp
  )

target_include_directories(clock_app
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/../robobrawl

  )

target_link_libraries(clock_app
  PUBLIC  robobrawl_interface
  PUBLIC  ws2811
  )

# make a copy of the QoS XML file in the current output directory
# [the app looks in current directory by default at runtime]
# TMS_COPY_QOS_XML_FILE( ${CMAKE_CURRENT_BINARY_DIR} APP_QOS_XML_FILE )
