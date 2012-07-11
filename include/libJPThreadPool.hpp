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
#include "libJPSemaphores.hpp"

#ifndef MAX_POOL_SIZE
#define MAX_POOL_SIZE 20
#endif

class JPPoolSharedMem{
private:
    JPBinSemaphore sem;
    
    /**
     * SharedMemory Part
     * Begin{
     */
    std::map<std::string,std::string> memory;
    /**
     * End}
     */
protected:
    std::string getFromMemory( std::string valName );
    int setToMemory( std::string valName , std::string value );
public:
    bool die();
    /**
     * This function will set the amount of
     * threads that need to die
     */
    int kill( int num );
};

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
         * 
         */
        JPSemaphore * poolSem;
        JPPoolSharedMem * shrMem;
    public:
        JPThrWorker( JPPoolSharedMem * shrMem );
        
        /**
         * 
         */
        int setFunction(thread_start_t funct);
        /**
         * 
         */
        int setArguments(var_t thread_args);
        /**
         * This will launch the runner function
         * on the thread that will execute
         */
        int run();
};


typedef std::map<std::string,JPThread*> thrpool_thr_t;
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
		var_t routineArgs;
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

	public:
            /**
             * Class constructor
             * @param poolSize Size of the pool
             * @param outSem Semaphore to sync the threads
             */
		MThreadPool( int poolSize = MAX_POOL_SIZE, MSemaphore * outSem = NULL);

                /**
                 * Function to set the routine to be called and the parameters
                 * @param start_routine Function for the threads to run
                 * @param arg Pointer to the arguments
                 * @return 0 In case of success
                 *         1 In case of error
                 */
                int setRoutine( void *(*start_routine) (void*),
				void *arg);
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

#endif
