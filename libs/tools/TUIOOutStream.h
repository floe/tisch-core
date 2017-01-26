/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _TUIOSTREAM_H_
#define _TUIOSTREAM_H_

#include <string>
#include <vector>

#include <TUIO2/TuioServer.h>
#include <BasicBlob.h>
#include <tisch.h>


class TISCH_SHARED TUIOOutStream {

	friend TISCH_SHARED TUIOOutStream& operator<< ( TUIOOutStream& s, const BasicBlob& b );

	public:

		TUIOOutStream( int mode = 0, const char* target = "127.0.0.1", int port = TISCH_PORT_CALIB );

		void start();
		void send();

	protected:

    TUIO2::TuioServer* server;

};


TISCH_SHARED TUIOOutStream& operator<< ( TUIOOutStream& s, const BasicBlob& b );

#endif // _TUIOSTREAM_H_

