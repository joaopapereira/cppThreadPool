CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(example_src exampleProgram.cpp) 
#SET(example1_src exampleProgram1.cpp) 
ADD_EXECUTABLE( exampleProgram ${example_src})
#ADD_EXECUTABLE( exampleProgram1 ${example1_src})
#
TARGET_LINK_LIBRARIES(exampleProgram ${LIBS} ${JPSEMAPHORES_LIBRARY} ${LOGGER_LIBRARY} pthread JPThreadPool)
#TARGET_LINK_LIBRARIES(exampleProgram1  ${CPPJPLOGGER_LIB} pthread JPSemaphores )

