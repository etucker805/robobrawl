#######################################################################
#
cmake_minimum_required(VERSION 3.28)
project (robobrawl_control)

# get the CoreDX DDS config:
include( ../cmake/coredx.cmake )

# build the TMS core libraries:
add_subdirectory( ../robobrawl ./robobrawl )


#copy font to build
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/gui/font.ttf
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ )


########################################################################
# Imgui setup 

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)

#######################################################################
#
# Here is an example application that implements a STORAGE device
# and utilizes the tms_interface library for the TMS implementation.
#
add_executable(control_app
  )

target_sources(control_app PUBLIC 
  main.cc
  init.cpp
  cleanup.cpp
  main_loop.cpp
  startup.cpp
  roles/control.cpp

  gui/gui_setup.cpp
  gui/gui_main_loop.cpp
  gui/gui_cleanup.cpp

  )

target_include_directories(control_app
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/../robobrawl
  )

target_link_libraries(control_app
  PUBLIC  robobrawl_interface
  PRIVATE imgui
  PRIVATE glfw
  )

# make a copy of the QoS XML file in the current output directory
# [the app looks in current directory by default at runtime]
# TMS_COPY_QOS_XML_FILE( ${CMAKE_CURRENT_BINARY_DIR} APP_QOS_XML_FILE )
