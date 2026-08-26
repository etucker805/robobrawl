#######################################################################
#
# This validates that the standard COREDX environment variables
# are present, and then establishes a few useful PATHS and LIB
# lists.
#

########################################
if ( NOT DEFINED ENV{COREDX_TOP} )
  message( FATAL_ERROR "Required variable not set: COREDX_TOP" )
endif()
if ( NOT DEFINED ENV{COREDX_TARGET} )
  message( FATAL_ERROR "Required variable not set: COREDX_TARGET" )
endif()
if ( NOT DEFINED ENV{COREDX_HOST} )
  message( FATAL_ERROR "Required variable not set: COREDX_HOST" )
endif()


FILE( TO_CMAKE_PATH   $ENV{COREDX_TOP}    COREDX_TOP    )
FILE( TO_CMAKE_PATH   $ENV{COREDX_TARGET} COREDX_TARGET )
FILE( TO_CMAKE_PATH   $ENV{COREDX_HOST}   COREDX_HOST   )

set( DDS_C_INC_PATH   ${COREDX_TOP}/target/include )
set( DDS_CC_INC_PATH  ${COREDX_TOP}/target/include )
set( DDS_CXX_INC_PATH ${COREDX_TOP}/target/include/dds_cxx )

set( DDS_LIB_PATH  ${COREDX_TOP}/target/${COREDX_TARGET}/lib )


########################################
# CoreDX DDS Libraries required
set(DDS_CXX_LIBS
  libdds_cxx_log.a
  libdds_qos_provider_log.a
  libdds_dyntype_log.a
  libdds_xml_log.a
  libdds_cf_log.a
  )

