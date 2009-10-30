/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <assert.h>
#include <exception>
#include <algorithm>

#include "BasicBlob.h"

BasicBlob::BasicBlob() { }

// load from istream
std::istream& operator>>( std::istream& s, BasicBlob& blob ) {

	s >> blob.pos.x >> blob.pos.y;
	s >> blob.size;
	s >> blob.id >> blob.pid;
	s >> blob.peak.x >> blob.peak.y;
	s >> blob.axis1.x >> blob.axis1.y;
	s >> blob.axis2.x >> blob.axis2.y;

	return s;
}

// dump to ostream
std::ostream& operator<<( std::ostream& s, BasicBlob& blob ) {

	s << blob.pos.x << " " << blob.pos.y << " ";
	s << blob.size << " ";
	s << blob.id << " " << blob.pid << " ";
	s << blob.peak.x << " " << blob.peak.y << " ";
	s << blob.axis1.x << " " << blob.axis1.y << " ";
	s << blob.axis2.x << " " << blob.axis2.y;

	return s;
}

