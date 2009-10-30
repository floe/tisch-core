/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _REGION_H_
#define _REGION_H_

#include <Vector.h>
#include <vector>

#include "Gesture.h"
#include "InputState.h"


#define REGION_FLAGS_VOLATILE 1<<INPUT_TYPE_COUNT


class TISCH_SHARED Region: public std::vector<Vector> {

	friend TISCH_SHARED std::istream& operator>> ( std::istream& s, Region& r );
	friend TISCH_SHARED std::ostream& operator<< ( std::ostream& s, Region& r );

	public:

		Region( int _flags = (1<<INPUT_TYPE_COUNT)-1 );

		int contains( Vector v );
		const int flags() const { return m_flags; }
		void flags( int _flags ) { m_flags = _flags; }

		std::vector<Gesture> gestures;

	protected:

		int m_flags;
};

// TODO: id should be read & written by StateRegion

// internal representation of a region with inputstate etc.
class TISCH_SHARED StateRegion: public Region {

	public:
		
		StateRegion( unsigned long long _id = -1 );

		StateRegion& operator=( Region& reg );

		void update(); // update all features from inputstate
		Gesture* nextMatch(); // return next matching gesture

		unsigned long long id;
		int first;
		InputState state;
		std::vector<Gesture>::iterator gst;
};

#endif // _REGION_H_

