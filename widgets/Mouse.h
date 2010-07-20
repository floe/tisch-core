/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MOUSE_H_
#define _MOUSE_H_

#include <map>

#include <Socket.h>
#include <BasicBlob.h>

class TISCH_SHARED Mouse {

	public:

		 Mouse( const char* target = "127.0.0.1" );
		~Mouse();

		void button ( int num, int button, int state, int x, int y );
		void motion ( int num, int x, int y );
		void passive( int num, int x, int y );
		void entry  ( int num, int state );

		void send_blobs();

	private:

		UDPSocket output;

		std::map<int,BasicBlob> blobs;
		int framenum;

};

#endif // _MOUSE_H_

