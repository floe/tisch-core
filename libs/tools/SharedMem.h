/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2013 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _SHAREDMEM_H_
#define _SHAREDMEM_H_

class SharedMem {

	public:

		SharedMem( int key, int size, bool create );
		~SharedMem();

    SharedMem( const SharedMem& r );
		SharedMem& operator=( const SharedMem& r );
	
		unsigned char* get(); 

    int acquire();
    int release();

	protected:

    int do_sem( short int val );

		unsigned char* buffer;
    int key, size;
    int shm, sem;
    bool create;

};

#endif // _SHAREDMEM_H_

