/**
 *  Copyright 2012 Joao Pereira<joaopapereira@gmail.com>
 *
 *
 *  This file is part of libJPThreadPool.
 *
 *  libJPThreadPool is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libJPSemaphores is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libJPSemaphores.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "libJPThreadPool.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#define MAXPOOL 50
const std::string JPThreadPool::moduleName = "THRP";

template <class T>
inline std::string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

/**
 * CLASS: JPPoolSharedMem
 * BEGIN {
 */
/**
 * Private methods
 */
/**
 * Protected methods
 */
/**
 * This function should NOT be overloaded
 * This function get a string from the shared memory
 * @param valName Key of what we want to get
 * @return String with the correspondent value or '\n'
 */
std::string
JPPoolSharedMem::getFromMemory( std::string valName ){
	std::string result;
	if( memory.find(valName) == memory.end() )
		result.assign("");
	else
		result = memory[valName];
	return result;
}
/**
 * This function should NOT be overloaded
 * Function changes a value of a key
 * @param valName Key to get value from
 * @param value Value of the key
 * @return 0 in cas os success
 */
int
JPPoolSharedMem::setToMemory( std::string valName , std::string value ){
	memory[valName] = value;
	return 0;
}
/**
 * Public methods
 */
/**
 * Class constructor that initialize the semaphore
 * and the memory
 */
JPPoolSharedMem::JPPoolSharedMem(Logger * log){

	this->log = log;
	log->log( "SHRM" ,M_LOG_MIN,M_LOG_TRC, "JPPoolSharedMem::JPPoolSharedMem(%p)",log);
}
static std::string dieKey("thr_kill");
/**
 * Function check if the thread need to die
 * @return True if thread need to die
 */
bool
JPPoolSharedMem::die(){
	log->log("SHRM",M_LOG_MIN,M_LOG_TRC,"JPPoolSharedMem::die()");
	bool result;
	sem.down();
	std::string val = getFromMemory( dieKey );
	if(0 == val.compare("") ){
		result = false;
	}else{
		int value = atoi(val.c_str());
		if( 0 == value ){
			result = false;
		}else{
			value--;
			result = true;
			if( 0 != setToMemory(dieKey,to_string(value)) )
				result = false;//this should never happen
		}
	}
	sem.up();
	log->log("SHRM",M_LOG_MIN,M_LOG_TRC,"JPPoolSharedMem::%s need to die",(result?"":"NO"));
	return result;
}
/**
 * This function will set the amount of
 * threads that need to die
 * @param num Number of threads to kill
 * @return 0 in case of success
 */
int
JPPoolSharedMem::kill( int num ){
	log->log("SHRM",M_LOG_MIN,M_LOG_TRC,"JPPoolSharedMem::kill(%d)",num);
	int result;
	sem.down();
	std::string val = getFromMemory( dieKey );
	if( 0 == val.compare("") ){
		if( 0 != setToMemory(dieKey,to_string(num)) )
				result = -1;//this should never happen
	}else{
		int value = atoi(val.c_str());
		value += num;
		if( 0 != setToMemory(dieKey,to_string(value)) )
			result = -2;//this should never happen
	}
	sem.up();
	return result;
}
/**
 * END }
 */




/**
 * CLASS: JPThrWorker
 * BEGIN {
 */
/**
 * Private methods
 */
/**
 * Protected methods
 */
/**
 * Public methods
 */
/**
 * Class constructor
 */
JPThrWorker::JPThrWorker( Logger * log, JPPoolSharedMem * shrMem, JPSemaphore * poolSem )
		    :JPThread(NULL){
	log->log("THRP",M_LOG_MIN,M_LOG_TRC,"JPThrWorker::JPThrWorker(%p,%p,%p)",log,shrMem,poolSem);
	this->shrMem = shrMem;
	this->poolSem = poolSem;
	waitOnFunction = true;
	waitOnArguments = true;
	somethingChanged = false;
	thread_args = new __thr_var_t;
	funct = NULL;
	logger = log;
	caller_args = new __worker_t;
	aux_args = new __thr_var_t;
}
/**
 * Class destructor
 */
JPThrWorker::~JPThrWorker(){
	delete aux_args;
	delete caller_args;
}

/**
 * Function used to change the function
 * that will be called in the next iteration of the
 * worker
 * @param funct Function pointer
 * @param waitOnArgs Indicates if the arguments will be changed after
 * @return Integer 0 on success
 */
int
JPThrWorker::setFunction(thread_start_t funct, bool waitOnArgs ){
	logger->log("THRP",M_LOG_MIN,M_LOG_TRC,"JPThrWorker::setFunction(%p,%d)",funct,(waitOnArgs?1:0));
	this->waitOnArguments = waitOnArgs;
	this->funct = funct;
	waitOnFunction = false;
	somethingChanged = true;
	return 0;
}
/**
 * Function used to change the arguments
 * that will be used in next iteration of the worker
 * @param thread_args Arguments
 * @param waitOnFunction Indicates if function will be changed after
 * @return Integer 0 on success
 */
int
JPThrWorker::setArguments(thr_var_t thread_args, bool waitOnFunction){
	logger->log("THRP",M_LOG_MIN,M_LOG_TRC,"JPThrWorker::setArguments(%p,%d)",thread_args,(waitOnFunction?1:0));
	this->waitOnFunction = waitOnFunction;
	//this->thread_args = thread_args;
	this->thread_args->funVariables = thread_args->funVariables;
	this->thread_args->logger = thread_args->logger;
	this->thread_args->thrName.assign(thread_args->thrName);
	waitOnArguments = false;
	somethingChanged = true;
	return 0;
}

/**
 * Forward declaration of function
 */
var_t doWork( var_t args );
/**
 * This will launch the runner function
 * on the thread that will execute
 */
int
JPThrWorker::start(){
	logger->log("THRP",M_LOG_MIN,M_LOG_TRC,"JPThrWorker::start()");
	caller_args->arg = (thr_var_t)thread_args;
	caller_args->funct = funct;
	caller_args->me = this;
	aux_args->logger = logger;
	aux_args->funVariables = (var_t*) caller_args;

	run( doWork, aux_args );
	return 0;
}
/**
 * Function called to check if
 * the current thread needs to die
 */
bool
JPThrWorker::needToDie(){
	return shrMem->die();
}
/**
 * Function called on the actual thread to wait for
 * something in pool semaphore
 */
void
JPThrWorker::waitForWork(){
	poolSem->down();
}

var_t
doWork( var_t args ){
	var_t lastResult;

	thr_var_t v = (thr_var_t)args;
	__worker_t * funs = (__worker_t * )v->funVariables;
	thr_var_t arg = (thr_var_t)funs->arg;
	Logger logger(v->logger);
    logger.log("THRP",M_LOG_MIN,M_LOG_TRC,"[%s]JPThrWorker::doWork(%p)",v->thrName.c_str(),v);
    funs->arg->logger = v->logger;
    funs->arg->thrName.assign(v->thrName);
	while(!funs->me->needToDie() ){
		logger.log("THRP",M_LOG_MIN,M_LOG_TRC,"[%s]JPThrWorker::doWork cycle using[%s]",v->thrName.c_str(),funs->arg->thrName.c_str());
		funs->me->waitForWork();
	    lastResult = funs->funct(funs->arg);
		logger.log("THRP",M_LOG_MIN,M_LOG_TRC,"[%s]JPThrWorker::doWork Function ended[%s]",v->thrName.c_str(),funs->arg->thrName.c_str());
	}
	logger.log("THRP",M_LOG_MIN,M_LOG_TRC,"[%s]JPThrWorker::doWork Ended...",v->thrName.c_str());

	delete funs;
}
/**
 * END }
 */




/**
 * CLASS: JPThrWorker
 * BEGIN {
 */
/**
 * Private methods
 */
/**
 * Protected methods
 */
/**
 * Public methods
 */
/**
 * Class constructor
 */
JPThreadPool::JPThreadPool( Logger * log, int poolSize , JPSemaphore * outSem ){
	logger = log;
	logger->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "JPThreadPool(%p,%d,%p)",log,poolSize,outSem );
	hasRoutine = 0;
	routine = NULL;
	maxPoolSize = poolSize;
	routineArgs = NULL;
	sem = outSem;
	shrMem = new JPPoolSharedMem( log );
	isPoolStarted = false;
}
/**
 * Class constructor
 */
JPThreadPool::~JPThreadPool(){
	logger->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "~JPThreadPool()" );
	delete shrMem;
	thrpool_thr_t::iterator it;
	for(it = pool.begin(); it != pool.end(); it++)
		delete it->second;
}

int
JPThreadPool::setRoutine(  thread_start_t start_routine,
                           thr_var_t arg){
	logger->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "setRoutine(%p,%p)",start_routine,arg );
	routine = (thread_start_t)start_routine;
	routineArgs = arg;

	return 0;
}

int
JPThreadPool::createPool ()
{
	logger->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "createPool()");
	if( routine == NULL )
		return -1;

	for( int i = 0 ; i < maxPoolSize && i < MAXPOOL ; i++ ){
		char aux[100];
		sprintf(aux,"thread-%d",i);
		launchThread( aux );
	}
	isPoolStarted = true;
	return 0;
}

int
JPThreadPool::launchThread( std::string str )
{
	logger->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "launchThread(%s)",str.c_str());
	pool[str] = new JPThrWorker(logger, shrMem, sem);
	pool[str]->setThreadName(str);
	pool[str]->setFunction(routine);
	pool[str]->setArguments(routineArgs);
	pool[str]->start();
	return 0;
}

int
JPThreadPool::setThreadNum( int num )
{
	logger->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "setThreadNum(%d)",num);
	if( isPoolStarted && ( maxPoolSize != num) ){
		if( maxPoolSize > num )
			shrMem->kill(maxPoolSize - num );
		else
			for( int i = maxPoolSize ; i < num && i < MAXPOOL ; i++ ){
					char aux[100];
					sprintf(aux,"thread-%d",i);
					launchThread( aux );
			}
	}
	maxPoolSize = num;
	return 0;
}

int
JPThreadPool::joinPool()
{
	logger->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "joinPool()");
	thrpool_thr_t::iterator it;
	for( it = pool.begin() ; it != pool.end() ; ++it ){
		this->sem->up();
		(*it).second->join();
	}
	return 0;
}
