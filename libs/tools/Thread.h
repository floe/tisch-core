/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _THREAD_H_
#define _THREAD_H_

#ifndef _MSC_VER
	#include <pthread.h>
	#include <semaphore.h>
#else

	#include <windows.h>

	#define pthread_t HANDLE
	#define sem_t HANDLE

	#define pthread_create(handle,attr,proc,arg) *handle = CreateThread(attr,0,(LPTHREAD_START_ROUTINE)proc,arg,0,0)
	#define pthread_join(handle,result) { WaitForSingleObject(handle,INFINITE); GetExitCodeThread(handle,(LPDWORD)result); }

	#define sem_init(handle,shared,value) *handle = CreateSemaphore(shared,value,0xFFFF,0)
	#define sem_wait(handle) WaitForSingleObject(*handle,INFINITE)
	#define sem_post(handle) ReleaseSemaphore(*handle,1,0)

#endif

class TISCH_SHARED Thread {

	public:

		Thread( );
		virtual ~Thread();

		virtual void* run();
		void start();

		void lock();
		void release();

	private:

		sem_t m_lock;
		pthread_t m_thread;

};


#endif // _THREAD_H_

