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

class TISCH_SHARED BasicBlob {

	friend TISCH_SHARED std::istream& operator>>( std::istream& s, BasicBlob& b );
	friend TISCH_SHARED std::ostream& operator<<( std::ostream& s, BasicBlob& b );

	public:

		BasicBlob();

		osc::int32 id, pid;
		int size, tracked;
		unsigned char value;

		::Vector pos, speed;
		::Vector peak, axis1, axis2;

};

#endif // _BASICBLOB_H_

