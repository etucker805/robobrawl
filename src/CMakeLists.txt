#######################################################################
#
cmake_minimum_required(VERSION 3.28)
project (robobrawl_total)

set(CMAKE_CXX_STANDARD 23)

include( ./cmake/coredx.cmake )


# set(COREDX_TOP "/home/ethan/dds/coredx-6.5.0-Evaluation")
# set(COREDX_HOST "Linux_2.6_x86_64_gcc11")

option(CONTROL          "add the control target" OFF)
option(BUTTON           "add the button_orange target" OFF)
option(CLOCK            "add the clock target" OFF)
option(LIGHTS           "add the lights target" OFF)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/robobrawl ./robobrawl)

if(CONTROL)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/control_app ./control)
endif()

if(BUTTON) # cool this works, no it dosnt...  defs? I guess
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/button_app ./button)
endif()

if(CLOCK)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/clock_app ./clock)
endif()

if(LIGHTS)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lights_app ./lights)
endif()
########################################################################