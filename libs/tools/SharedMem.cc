/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2013 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "SharedMem.h"

#include <string>
#include <string.h>

#include <errno.h>
#include <stdexcept>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

SharedMem::SharedMem( int _key, int _size, bool _create ) {

	key = _key;
	size = _size;
	create = _create;

	#ifndef _MSC_VER

		int flags = 0666;
		if (create) flags |= IPC_CREAT | IPC_EXCL;

		shm = shmget( key, size, flags );
		sem = semget( key,    1, flags );

		buffer = (unsigned char*) shmat( shm, 0, 0 );
		if (buffer == (void*)-1)
			throw std::runtime_error( std::string("shmat: ") + std::string(strerror(errno)) );

		if (create) semctl( sem, 0, SETVAL, 1 );

	#else

		char name[64];

		snprintf(name,sizeof(name),"libTISCH-SHM-0x%08X",key);
		shm = CreateFileMapping( (HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, size, name );

		snprintf(name,sizeof(name),"libTISCH-SEM-0x%08X",key);
		sem = CreateSemaphore( NULL, 1, 1, name );

		buffer = MapViewOfFile( handle, FILE_MAP_WRITE, 0, 0, size );

	#endif
}

SharedMem::~SharedMem() {
	if (create) {
	#ifndef _MSC_VER
		struct shmid_ds shmstat;
		shmdt( (void*)buffer );
		shmctl( shm, IPC_RMID, &shmstat );
		semctl( sem, 0, IPC_RMID );
	#else
		UnmapViewOfFile( buffer );
	#endif
	}
}

SharedMem::SharedMem( const SharedMem& r ):
	buffer(r.buffer), key(r.key), size(r.size),
	shm(r.shm), sem(r.sem), create(false) { }
	
SharedMem& SharedMem::operator=( const SharedMem& r ) {
	if (&r == this) return *this;
	buffer = r.buffer; key = r.key; size = r.size;
	shm = r.shm; sem = r.sem; create = false;
	return *this;
}

unsigned char* SharedMem::get() { return buffer; }

// wrapper for semaphore access
#ifndef _MSC_VER
int SharedMem::do_sem( short int val ) {
	struct sembuf sem_op = { 0, val, 0 };
	return semop( sem, &sem_op, 1 );
}
#else
int SharedMem::do_sem( short int val ) {
	if (val > 0) return ReleaseSemaphore( sem, val, NULL );
	else return WaitForSingleObject( sem, INFINITE );
}
#endif

int SharedMem::acquire() { return do_sem( -1 ); }
int SharedMem::release() { return do_sem( +1 ); }

