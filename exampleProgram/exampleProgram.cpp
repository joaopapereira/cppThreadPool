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
#include <libJPLogger.hpp>
#include <libJPSemaphores.hpp>
#include "libJPThreadPool.hpp"

#include <unistd.h>
#include <stdlib.h>
#include <iostream>
using namespace jpCppLibs;

bool threadsDie = false;
#define MAX_NUMBER_OF_THREADS 5

JPSemaphore  * sem;
void * doStuff( void * args ){
	thr_var_t myArgs = (thr_var_t)args;
	Logger* myLogger = myArgs->logger;
	myLogger->log("MYTHR",M_LOG_NRM, M_LOG_DBG,"IN....%s",myArgs->thrName.c_str());
	myLogger->log("MYTHR",M_LOG_NRM, M_LOG_DBG,"I need to write: %s",((std::string*)myArgs->funVariables)->c_str());
	sleep(1);
	myLogger->log("MYTHR",M_LOG_NRM, M_LOG_DBG,"OUT....%s",myArgs->thrName.c_str());
}

void * killAll( void * args ){
	thr_var_t myArgs = (thr_var_t)args;
	Logger* myLogger = myArgs->logger;
	myLogger->log("killAll",M_LOG_NRM, M_LOG_DBG,"I will wait for 10 seconds before killing thread pool");
	sleep(10);
	((JPThreadPool*)myArgs->funVariables)->setThreadNum(0);
	threadsDie = true;
	for( int i = 0 ; i < MAX_NUMBER_OF_THREADS ; i++ ) sem->up();
	myLogger->log("killAll",M_LOG_NRM, M_LOG_DBG,"Thread Killer will die now");

}

void * randomStuffHappening( void * args ){
	thr_var_t myArgs = (thr_var_t)args;
	Logger* myLogger = myArgs->logger;
	myLogger->log("MYTHR",M_LOG_NRM, M_LOG_DBG,"This thread asks for job done");
	while(!threadsDie){
		usleep(rand()%500000);
		myLogger->log("MYTHR",M_LOG_NRM, M_LOG_DBG,"Asking for job");
		sem->up();
	}
}

int main(void) {
  Logger * log = new Logger();
  log->setFile("/tmp/test.log");

  log->setLogLvl("SEM", M_LOG_MIN , M_LOG_ALLLVL);
  log->setLogLvl("MYTHR", M_LOG_MIN , M_LOG_ALLLVL);
  log->setLogLvl("THRP", M_LOG_MIN , M_LOG_ALLLVL);
  log->setLogLvl("SHRM", M_LOG_MIN , M_LOG_ALLLVL);
  log->setLogLvl("killAll", M_LOG_MIN , M_LOG_ALLLVL);
  log->setLogLvl("MAIN", M_LOG_MIN , M_LOG_ALLLVL);
  sem = new JPSemaphore (MAX_NUMBER_OF_THREADS,0);
  JPThreadPool pool(log , MAX_NUMBER_OF_THREADS, sem );
  thr_var_t args = new __thr_var_t;
  thr_var_t argsK = new __thr_var_t;
  thr_var_t argsR = new __thr_var_t;
  args->logger = log;
  argsK->logger = log;
  argsR->logger = log;
  args->funVariables = (var_t*)new std::string("Stuff to write");

  pool.setRoutine( doStuff , args);

  pool.createPool();

  argsK->funVariables = (var_t*)&pool;
  JPThread thr(killAll);
  thr.run(argsK);

  JPThread thr1(randomStuffHappening);
  thr1.run(argsR);

  pool.joinPool();
  log->log("Pool closed","MAIN",M_LOG_NRM,M_LOG_DBG);

  thr.join();
  log->log("Kill all closed","MAIN",M_LOG_NRM,M_LOG_DBG);
  thr1.join();
  log->log("Job creator closed","MAIN",M_LOG_NRM,M_LOG_DBG);


  OneInstanceLogger::instance()->log("Ended APP","SEM",M_LOG_NRM,M_LOG_DBG);
  return 0;
}
