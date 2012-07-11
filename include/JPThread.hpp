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

typedef void* var_t;
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
         * Function that starts the thread
         * @param thread_args Arguments to send to function
         */
        void run(var_t thread_args);
        /**
         * Wait for thread to die
         */
        void join();

    private:
        /**
         * Function to run
         */
        thread_start_t _thread_start;
        /**
         * Thread id
         */
        pthread_t _thread;


};

#endif	/* JPTHREAD_HPP */

