CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
SET(CMAKE_VERBOSE_MAKEFILE ON)
MESSAGE(STATUS "CMake version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

PROJECT(OMSICSimulationRuntime)

# enable warnings and use ANSI C compatible compiler
if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
  set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra -ansi -pedantic")
endif(CMAKE_COMPILER_IS_GNUCXX)

if(NOT PLATFORM OR PLATFORM STREQUAL "dynamic")
  set(BUILD_SHARED_LIBS ON)
elseif(PLATFORM STREQUAL "static")
  set(BUILD_SHARED_LIBS OFF)
else()
endif()

IF(BUILD_SHARED_LIBS)
  SET(LIBSUFFIX "")
ELSE(BUILD_SHARED_LIBS)
  SET(LIBSUFFIX "_static")
ENDIF(BUILD_SHARED_LIBS)

 IF(MSVC)
  MESSAGE(STATUS "MSVC")
  SET(LIBINSTALLEXT "lib/omc/omsi/msvc" CACHE STRING "library directory")
ELSE(MSVC)
  SET(LIBINSTALLEXT "lib/omc/omsi" CACHE STRING "library directory")
 ENDIF(MSVC)

SET(OSUCName ${LIBPREFIX}OMSIC${LIBSUFFIX})

SET(OMSI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../OMSI)

include_directories ("${OMSI_SOURCE_DIR}/include")
include_directories ("${OMSI_SOURCE_DIR}/base/include")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

# omsic simulation runtime
ADD_SUBDIRECTORY(src/solver)
#ADD_SUBDIRECTORY(omsu)

#target_link_libraries(${OSUCName}  ${CMAKE_DL_LIBS} )

#install(TARGETS ${OSUCName} DESTINATION ${LIBINSTALLEXT})
