CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
PROJECT(OMSI base simulation runtime)

install(FILES
  ${CMAKE_SOURCE_DIR}/include/omsi.h
  ${CMAKE_SOURCE_DIR}/include/omsi_callbacks.h
  ${CMAKE_SOURCE_DIR}/include/omsi_cs.h
  ${CMAKE_SOURCE_DIR}/include/omsi_eqns_system.h
  ${CMAKE_SOURCE_DIR}/include/omsi_jacobian.h
  ${CMAKE_SOURCE_DIR}/include/omsi_me.h
DESTINATION include/OMSI)
install(FILES
  ${CMAKE_SOURCE_DIR}/include/fmi2/omsi_fmi2_cs.h
  ${CMAKE_SOURCE_DIR}/include/fmi2/omsi_fmi2_me.h
  DESTINATION include/OMSI/fmi2/)





