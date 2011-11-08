/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Thread.h"

#include <iostream>


static void* g_dispatcher(void* arg) {
	Thread* target = (Thread*)arg;
	return target->run();
}

Thread::Thread( ) {
	sem_init(&m_lock,0,1);
}

void Thread::start() {
	pthread_create(&m_thread,0,&g_dispatcher,(void*)this);
}

Thread::~Thread() {
	void* result;
	pthread_join(m_thread,&result);
}

void* Thread::run() { std::cout << "Thread::run()" << std::endl; return 0; }

void Thread::lock()    { sem_wait(&m_lock); }
void Thread::release() { sem_post(&m_lock); }

