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
#ifndef libJPThreadPool_H
#define libJPThreadPool_H
#include "JPThread.hpp"
#include <libJPSemaphores.hpp>
#include <libJPLogger.hpp>
#include <string>

namespace jpCppLibs{
	#ifndef MAX_POOL_SIZE
	#define MAX_POOL_SIZE 20
	#endif
	
	/**
	 * Shared memory class used to set share variables between threads
	 */
	class JPPoolSharedMem{
	private:
		/**
		 * Semaphore to access the memory
		 */
	    JPBinSemaphore sem;
	    /**
	     * Logger
	     */
	    Logger * log;
	    /**
	     * SharedMemory Part
	     * Begin{
	     */
	    std::map<std::string,std::string> memory;
	    /**
	     * End}
	     */
	protected:
	    /**
	     * This function should NOT be overloaded
	     * This function get a string from the shared memory
	     * @param valName Key of what we want to get
	     * @return String with the correspondent value or '\n'
	     */
	    std::string getFromMemory( std::string valName );
	    /**
	     * This function should NOT be overloaded
	     * Function changes a value of a key
	     * @param valName Key to get value from
	     * @param value Value of the key
	     * @return 0 in case of success
	     */
	    int setToMemory( std::string valName , std::string value );
	public:
	    /**
	     * Class constructor that initialize the semaphore
	     * and the memory
	     */
	    JPPoolSharedMem( Logger * log );
	    /**
	     * Function check if the thread need to die
	     * @return True if thread need to die
	     */
	    bool die();
	    /**
	     * This function will set the amount of
	     * threads that need to die
	     * @param num Number of threads to kill
	     * @return 0 in case of success
	     */
	    int kill( int num );
	};
	
	class JPThrWorker;
	/**
	 * worker structure
	 */
	typedef struct{
		thr_var_t arg;
		thread_start_t funct;
		JPThrWorker * me;
	} __worker_t;
	/**
	 * This class will implement a worker
	 * instead of the control over the thread rely on the
	 * thread function the control will rely in this class
	 * that will wait for instructions from the Pool end
	 * This class will also allow the the function to be executed 
	 * to change during the process
	 */
	class JPThrWorker: public JPThread{
		protected:
			static void * runner( void * thrArgs );
			/**
			 * Semaphore from the pool
			 */
			JPSemaphore * poolSem;
			/**
			 * Shared memory
			 */
			JPPoolSharedMem * shrMem;
			/**
			 * Function to call
			 */
			thread_start_t funct;
			/**
			 * Arguments to the function
			 */
			thr_var_t thread_args;
			/**
			 * Arguments to the function
			 */
			thr_var_t aux_args;
	
			/**
			 * Arguments of the caller function
			 */
			__worker_t * caller_args;
			/**
			 * Should wait for the function
			 */
			bool waitOnFunction;
			/**
			 * Should wait for the arguments
			 */
			bool waitOnArguments;
			/**
			 * Changes happened
			 */
			bool somethingChanged;
			/**
			 * Logger
			 */
			Logger * logger;
	
		public:
			/**
			 * Class constructor
			 */
			JPThrWorker( Logger * log, JPPoolSharedMem * shrMem, JPSemaphore * poolSem );
			/**
			 * Class destructor
			 */
			~JPThrWorker( );
	
			/**
			 * Function used to change the function
			 * that will be called in the next iteration of the
			 * worker
			 * @param funct Function pointer
			 * @param waitOnArgs Indicates if the arguments will be changed after
			 * @return Integer 0 on success
			 */
			int setFunction(thread_start_t funct, bool waitOnArgs = false);
			/**
			 * Function used to change the arguments
			 * that will be used in next iteration of the worker
			 * @param thread_args Arguments
			 * @param waitOnFunction Indicates if function will be changed after
			 * @return Integer 0 on success
			 */
			int setArguments(thr_var_t thread_args, bool waitOnFunction = false);
			/**
			 * This will launch the runner function
			 * on the thread that will execute
			 */
			int start();
			/**
			 * Function called to check if
			 * the current thread needs to die
			 */
			bool needToDie();
			/**
			 * Function called on the actual thread to wait for
			 * something in pool semaphore
			 */
			void waitForWork();
	};
	
	
	typedef std::map<std::string,JPThrWorker*> thrpool_thr_t;
	/**
	 * Class that implements a pool of threads
	 */
	class JPThreadPool{
	
		private:
			/**
			 * Variable that contains the pool of threads
			 */
			thrpool_thr_t pool;
			/**
			 * Maximum size of the pool
			 */
			int	maxPoolSize;
			/**
			 * Variable that says if a routine as been
			 * setted
			 */
			int	hasRoutine;
			/**
			 * Function to run on the pool
			 */
			thread_start_t routine;
			/**
			 * Arguments for the function
			 */
			thr_var_t routineArgs;
			/**
			 * Semaphore used to syncronize the threads
			 */
			JPSemaphore *sem;
			/**
			 * Variable to be used on the logger
			 */
			static const std::string moduleName;
	
			/**
			 * Function to launch one thread
			 * @param str Name of the thread
			 * @return 0 In case of success
			 *         1 In case of error
			 */
			int launchThread( std::string str );
			/**
			 * Logger
			 */
			Logger * logger;
			/**
			 * Pool was started
			 */
			bool isPoolStarted;
	    protected:
			/**
			 * Shared Memory
			 * This is in protected to allow classes
			 * to add different types of shrmem
			 */
			JPPoolSharedMem * shrMem;
		public:
			/**
			 * Class constructor
			 * @param poolSize Size of the pool
			 * @param outSem Semaphore to sync the threads
			 */
			JPThreadPool( Logger * log, int poolSize = MAX_POOL_SIZE, JPSemaphore * outSem = NULL);
			/**
			 * Class destructor
			 */
			~JPThreadPool();
	
			/**
			 * Function to set the routine to be called and the parameters
			 * @param start_routine Function for the threads to run
			 * @param arg Pointer to the arguments
			 * @return 0 In case of success
			 *         1 In case of error
			 */
			int setRoutine( thread_start_t start_routine,
					        thr_var_t arg);
			/**
			 * Function used to create the pool
			 * @return 0 In case of success
			 *         1 In case of error
			 */
			int createPool();
			/**
			 * Change the number of threads to be launched
			 * @param num Number of threads to be launched
			 * @return 0 In case of success
			 *         1 In case of error
			 */
			int setThreadNum( int num );
	
			/**
			 * This function will wait until all threads are dead
			 * @return 0 In case of success
			 *         1 In case of error
			 */
			int joinPool();
	
	};
};	
#endif
