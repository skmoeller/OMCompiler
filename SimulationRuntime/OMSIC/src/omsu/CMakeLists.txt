CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
#OpenModelica Simulation Unit C Runtime
PROJECT(OMSU)
SET(CMAKE_VERBOSE_MAKEFILE ON)

set(OMSUName ${LIBPREFIX}OMSU${LIBSUFFIX})

#set OMSIBase lib
if(NOT PLATFORM OR PLATFORM STREQUAL "dynamic")
  set (libOMSIBase ${OMSI_SOURCE_DIR}/Build_${PLATFORM}/base/libOMSIBase.dll)
elseif(PLATFORM STREQUAL "static")
  set (libOMSIBase ${OMSI_SOURCE_DIR}/Build_${PLATFORM}/base/libOMSIBase_static.a)
else()
endif()


include_directories ("${CMAKE_SOURCE_DIR}/include/solver")
include_directories ("${CMAKE_SOURCE_DIR}/include/omsu")
include_directories ("${OMSI_SOURCE_DIR}/base/include")

add_library(${OMSUName} omsu_helper.c omsu_utils.c omsu_initialization.c)

target_link_libraries(${OMSUName} ${CMAKE_DL_LIBS} ${libOMSIBase})

install(TARGETS ${OMSUName} DESTINATION ${LIBINSTALLEXT})
