#######################################################################
#
cmake_minimum_required(VERSION 3.28)
project (robobrawl_control)

# get the CoreDX DDS config:
include( ../cmake/coredx.cmake )

# build the TMS core libraries:
# add_subdirectory( ../robobrawl s./robobrawl )


#######################################################################
#
# Here is an example application that implements a STORAGE device
# and utilizes the tms_interface library for the TMS implementation.
#
add_executable(button_app
  )

target_sources(button_app PUBLIC 
  main.cc
  init.cpp
  cleanup.cpp
  main_loop.cpp
  startup.cpp
  roles/button.cpp
  )

target_include_directories(button_app
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/../robobrawl
  )

target_link_libraries(button_app
  PUBLIC  robobrawl_interface
  )

# make a copy of the QoS XML file in the current output directory
# [the app looks in current directory by default at runtime]
# TMS_COPY_QOS_XML_FILE( ${CMAKE_CURRENT_BINARY_DIR} APP_QOS_XML_FILE )
