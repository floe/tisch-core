/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BASICBLOB_H_
#define _BASICBLOB_H_

#include <iostream>
#include <osc/OscTypes.h>
#include "Vector.h"


// TODO: these should be TUIO 2.0 types
enum InputType {
	INPUT_TYPE_FINGER,
	INPUT_TYPE_HAND,
	INPUT_TYPE_SHADOW,
	INPUT_TYPE_OBJECT,
	INPUT_TYPE_OTHER,
	// always keep as last 
	INPUT_TYPE_COUNT
};


class TISCH_SHARED BasicBlob {

	friend TISCH_SHARED std::istream& operator>>( std::istream& s, BasicBlob& b );
	friend TISCH_SHARED std::ostream& operator<<( std::ostream& s, BasicBlob& b );

	public:

		BasicBlob();

		osc::int32 id, pid, type;
		int size, tracked;
		unsigned char value;

		::Vector pos, speed;
		::Vector peak, axis1, axis2;

};

#endif // _BASICBLOB_H_

