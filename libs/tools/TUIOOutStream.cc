/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "TUIOOutStream.h"
#include <time.h>


TUIOOutStream::TUIOOutStream( int _mode, const char* target, int port ):
	server(new TUIO2::TuioServer())
{ }


void TUIOOutStream::start() {
	server->initTuioFrame(TUIO2::TuioTime::getSessionTime());
}


TUIOOutStream& operator<< ( TUIOOutStream& s, const BasicBlob& b ) {


	return s;
}


void TUIOOutStream::send() {
	server->commitTuioFrame();
}

