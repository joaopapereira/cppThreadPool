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
#include <extlibs/libJPLogger.h>
#include <iostream>
#include <stdio.h>
#define MAXPOOL 50
const std::string JPThreadPool::moduleName = "THRP";

JPThreadPool::JPThreadPool( int poolSize , MSemaphore * outSem )
{
	MLog::instance()->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "JPThreadPool(%d,%p)",poolSize,outSem );
	hasRoutine = 0;
	routine = NULL;
	maxPoolSize = poolSize;
	routineArgs = NULL;
	sem = outSem;
}

int
JPThreadPool::setRoutine( void *(*start_routine) (void*),
			void *arg)
{
	MLog::instance()->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "setRoutine(%p,%p)",start_routine,arg );
	routine = (mom::thread_start_t)start_routine;
	routineArgs = (mom::var_t)arg;

	return success;
}

int
JPThreadPool::createPool ()
{
	MLog::instance()->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "createPool()");
	if( routine == NULL )
		return error;

	for( int i = 0 ; i < maxPoolSize && i < MAXPOOL ; i++ ){
		char aux[100];
		sprintf(aux,"thread-%d",i);
		launchThread( aux );
	}
	return success;
}

int
JPThreadPool::launchThread( std::string str )
{
	MLog::instance()->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "launchThread(%s)",str.c_str());
	MThread * thr = new MThread(routine);
	pool[str] = thr;
	((m_thrArgs*)(void*)routineArgs)->name->assign( str );
	pool[str]->Start( routineArgs );
	return success;
}

int
JPThreadPool::setThreadNum( int num )
{
	MLog::instance()->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "setThreadNum(%d)",num);
	maxPoolSize = num;
	return success;
}

int
JPThreadPool::joinPool()
{
	MLog::instance()->log( moduleName ,M_LOG_MIN,M_LOG_TRC, "joinPool()");
	thrpool_thr_t::iterator it;
	for( it = pool.begin() ; it != pool.end() ; ++it ){
		this->sem->up();
		(*it).second->Join();
	}
	return success;
}
