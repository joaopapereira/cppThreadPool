/*
 ============================================================================
 Name        : exampleProgram.c
 Author      : Joao Pereira
 Version     :
 Copyright   : This library is creating under the GPL license
 Description : Uses shared library to print greeting
               To run the resulting executable the LD_LIBRARY_PATH must be
               set to ${project_loc}/libJPSemaphores/.libs
               Alternatively, libtool creates a wrapper shell script in the
               build directory of this program which can be used to run it.
               Here the script will be called exampleProgram.
 ============================================================================
 */
#include <extlibs/libJPLogger.h>
#include <extlibs/libJPSemaphores.hpp>
#include "libJPThreadPool.hpp"

#include <unistd.h>
#include <iostream>

void * doStuff( void * args ){
	thr_var_t myArgs = (thr_var_t)args;
	Logger* myLogger = myArgs->logger;
	myLogger->log("MYTHR",M_LOG_NRM, M_LOG_DBG,"IN....%s",myArgs->thrName->c_str());
	myLogger->log("MYTHR",M_LOG_NRM, M_LOG_DBG,"OUT....%s",myArgs->thrName->c_str());
}


int main(void) {
  Logger log("/tmp/test.log");
  log.setLogLvl("SEM", M_LOG_MIN , M_LOG_ALLLVL);
  log.setLogLvl("MYTHR", M_LOG_MIN , M_LOG_ALLLVL);
  log.setLogLvl("THRP", M_LOG_MIN , M_LOG_ALLLVL);
  JPSemaphore sem(5,0);
  JPThreadPool pool(&log , 5, &sem );
  thr_var_t args;

  pool.setRoutine( doStuff , args);

  pool.createPool();

  pool.joinPool();


  OneInstanceLogger::instance()->log("Ended APP","SEM",M_LOG_NRM,M_LOG_DBG);
  return 0;
}
