CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
SET(CMAKE_VERBOSE_MAKEFILE ON)
MESSAGE(STATUS "CMake version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

PROJECT(OMSIBaseSimulationRuntime)


SET(OSUBaseName ${LIBPREFIX}OSIBase${LIBSUFFIX})
SET(LIBINSTALLEXT "lib/omc/omsi" CACHE STRING "library directory")
include_directories ("${CMAKE_SOURCE_DIR}/include")
# omsi base simulation runtime
ADD_SUBDIRECTORY(base)

install(FILES
  ${CMAKE_SOURCE_DIR}/include/omsi.h
  ${CMAKE_SOURCE_DIR}/include/omsi_callbacks.h
  ${CMAKE_SOURCE_DIR}/include/omsi_cs.h
  ${CMAKE_SOURCE_DIR}/include/omsi_jacobian.h
  ${CMAKE_SOURCE_DIR}/include/omsi_me.h
DESTINATION include/OMSI)
install(FILES
  ${CMAKE_SOURCE_DIR}/include/fmi2/omsi_fmi2_cs.h
  ${CMAKE_SOURCE_DIR}/include/fmi2/omsi_fmi2_me.h
  DESTINATION include/OMSI/fmi2/)





