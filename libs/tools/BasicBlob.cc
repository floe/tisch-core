/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <assert.h>
#include <exception>
#include <algorithm>

#include "BasicBlob.h"

BasicBlob::BasicBlob():
	pos(0,0,0),
	speed(0,0,0),
	peak(0,0,0),
	axis1(1,0,0),
	axis2(0,1,0)
{
	size = 1;
	type = 0;
	value = 1;
	tracked = 0;
	id = pid = 0;
}

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

