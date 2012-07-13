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
#include "JPThread.hpp"


/**
 * Function implements the constructor of the
 * class
 * @param thread_start The function to be called
 */
JPThread::JPThread(thread_start_t thread_start)
		 :_thread_name("Thread"){
    _thread_start = thread_start;
}

/**
 * Function that starts the thread
 * @param thread_args Arguments to send to function
 */
void
JPThread::run(thr_var_t thread_args) {
	if( NULL == _thread_start )
		return;
	if( NULL == thread_args->logger)
		thread_args->logger = new Logger();
	//if( NULL == thread_args->thrName )
	thread_args->thrName = new std::string(_thread_name);
	//else if( 0 == thread_args->thrName->size() )
	//	thread_args->thrName->assign( _thread_name );
	thread_args->logger->log("THRP",M_LOG_MIN,M_LOG_TRC,"JPThread::run(%p) calling pthread_create",thread_args);
    pthread_create(&_thread, NULL, _thread_start, (void*)thread_args);
}
void
JPThread::run(thread_start_t thread_start, thr_var_t thread_args) {
	_thread_start = thread_start;
	run( thread_args );
}
void
JPThread::join() {
    pthread_join(_thread, NULL);
}
/**
 * Set the name of the
 * @param name Name of the thread
 */
void
JPThread::setThreadName(std::string name){
	this->_thread_name = name;
}
