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

#ifndef JPTHREAD_HPP
#define	JPTHREAD_HPP
#include <pthread.h>
#include <string>
#include <libJPLogger.hpp>

namespace jpCppLibs{
	
	typedef void* var_t;
	typedef struct{
		var_t funVariables;
		Logger * logger;
		std::string thrName;
	}__thr_var_t;
	
	typedef __thr_var_t * thr_var_t;
	
	typedef var_t (*thread_start_t)(var_t);
	/**
	 * This class implements a single thread
	 */
	class JPThread
	{
	
	    public:
	        /**
	         * Function implements the constructor of the
	         * class
	         * @param thread_start The function to be called
	         */
	        JPThread(thread_start_t thread_start);
	        /**
			 * Function implements the destructor of the
			 * class
			 */
			virtual ~JPThread();
	        /**
	         * Function that starts the thread
	         * @param thread_args Arguments to send to function
	         */
	        void run(thr_var_t thread_args);
	        /**
			 * Function that starts the thread
			 * @param thread_start_t Function to be called
			 * @param thread_args Arguments to send to function
			 */
			void run(thread_start_t thread_start, thr_var_t thread_args);
	        /**
	         * Wait for thread to die
	         */
	        void join();
	
	        /**
	         * Set the name of the
	         * @param name Name of the thread
	         */
	        void setThreadName(std::string name);
	
	    private:
	        /**
	         * Function to run
	         */
	        thread_start_t _thread_start;
	        /**
	         * Thread id
	         */
	        pthread_t _thread;
	        /**
	         * Name of the thread
	         */
	        std::string _thread_name;
	        /**
	         * arguments to be passed to the thread
	         */
	        thr_var_t _thread_args;
	        /**
	         * Logger created
	         */
	        bool createdLogger;
	
	};
};
#endif	/* JPTHREAD_HPP */

