#######################################################################
#
cmake_minimum_required(VERSION 3.28)
project (intracom_tms_storage)

# get the CoreDX DDS config:
include( ../cmake/coredx.cmake )

# build the TMS core libraries:
add_subdirectory( ../tms ./tms )


#######################################################################
#
# Here is an example application that implements a STORAGE device
# and utilizes the tms_interface library for the TMS implementation.
#
add_executable(storage_app
  )

target_sources(storage_app PUBLIC 
  main.cc
  init.cpp
  cleanup.cpp
  main_loop.cpp
  startup.cpp
  roles/intracom_storage.cpp
  )

target_include_directories(storage_app
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/../tms
  )

target_link_libraries(storage_app
  PUBLIC  tms_interface
  )

# make a copy of the QoS XML file in the current output directory
# [the app looks in current directory by default at runtime]
TMS_COPY_QOS_XML_FILE( ${CMAKE_CURRENT_BINARY_DIR} APP_QOS_XML_FILE )
