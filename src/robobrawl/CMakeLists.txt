#######################################################################
#
#  Builds the core tms libraries (tms_core and tms_interface).
#
#  These libraries implement the required TMS message structures
#  and instantiate the DDS DataReaders and DataWriters to
#  facilitate communication.  This layer insulates the application
#  from details of the DDS middleware.

set( ROBOBRAWL_SRC ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "" )

#######################################################################
#
# Generate code from the 'tms.idl' file, and produce library 'tms_core'
#
set( ROBOBRAWL_GENERATED_SRCS
  generated/robobrawl.cc
  generated/robobrawl.hh
  generated/robobrawlTypeSupport.cc
  generated/robobrawlTypeSupport.hh
  )
set( ROBOBRAWL_IDL ${ROBOBRAWL_SRC}/idl/robobrawl.idl )

add_custom_command( 
  OUTPUT  ${ROBOBRAWL_GENERATED_SRCS}
  COMMAND cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated
  COMMAND ${COREDX_TOP}/host/${COREDX_HOST}/bin/coredx_ddl -l cpp11 -d generated/ -f ${TMS_IDL}
  MAIN_DEPENDENCY ${TMS_IDL}
  )

set_source_files_properties(
  ${TMS_GENERATED_SRCS}
  PROPERTIES GENERATED TRUE)

add_library(robobrawl_core STATIC
  ${TMS_GENERATED_SRCS}
  )

target_include_directories(robobrawl_core
  SYSTEM PUBLIC
  $<BUILD_INTERFACE:${DDS_CXX_INC_PATH}>
  $<BUILD_INTERFACE:${DDS_C_INC_PATH}>
  )

target_link_directories(robobrawl_core
  PUBLIC
  ${DDS_LIB_PATH}
  )
target_link_libraries(robobrawl_core
  PUBLIC
  ${DDS_CXX_LIBS}
  )

set_target_properties(robobrawl_core
  PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:robobrawl_core,INTERFACE_INCLUDE_DIRECTORIES>
  )

#######################################################################
#
# Produce the library 'robobrawl_interface'
#
#  This library uses tms_core, and adds some basic
#  TMS related functionality
#
# -- much of this layer is implemented in header (.hpp) files
#
add_library(robobrawl_interface STATIC
  
  components/active_mc.cpp
  components/robobrawl_context.cpp

  device/known_devices.cpp
  device/robobrawl_base.cpp
  device/robobrawl_device.cpp
  device/robobrawl_storage.cpp

  )

target_include_directories(robobrawl_interface
  PUBLIC
  ${ROBOBRAWL_SRC}/..
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  $<BUILD_INTERFACE:${DDS_CXX_INC_PATH}>
  $<BUILD_INTERFACE:${DDS_C_INC_PATH}>
  )

target_link_libraries( robobrawl_interface
  PUBLIC robobrawl_core
  )

#######################################################################
#
# This provides a simple macro to copy the milstd QoS XML file into
# a target directory. (The file is loaded at run-time to configure
# DDS entity Quality-of-Service configuration.)
#

# input:  target_dir
# output: installed_qos_file (full path to newly copied qos file)
macro( TMS_COPY_QOS_XML_FILE target_dir installed_qos_file )
  
  set( qos_src_path ${ROBOBRAWL_SRC}/qos/v2.0.2 )
  set( qos_file     mil-std-3071_qos_library.xml )
  
  add_custom_command(
    OUTPUT ${target_dir}/${qos_file}
    # make sure dest path exists:
    COMMAND ${CMAKE_COMMAND} -E copy ${qos_src_path}/${qos_file} ${target_dir}/${qos_file}
    DEPENDS ${qos_src_path}/${qos_file}
    COMMENT "Prepare ${target_dir}/${qos_file}"
    VERBATIM )
  

  set_source_files_properties(${target_dir}/${qos_file} PROPERTIES GENERATED true)
  add_custom_target( ${qos_file}_gen
    ALL
    DEPENDS  ${target_dir}/${qos_file} )
  
  set( ${installed_qos_file} ${target_dir}/${qos_file} )
    
endmacro()