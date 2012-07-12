
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET( lib_srcs thread.cpp threadPool.cpp )
include_directories( include/ ${INSTALL_INCS})
ADD_LIBRARY( JPThrealPool SHARED ${lib_srcs})

