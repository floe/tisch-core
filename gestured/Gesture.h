/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _GESTURE_H_
#define _GESTURE_H_

#include <string>
#include <vector>
#include <deque>
#include <map>

#include <BasicBlob.h>
#include <SmartPtr.h>

#include "Feature.h"


#define GESTURE_FLAGS_STICKY  1
#define GESTURE_FLAGS_ONESHOT 2
#define GESTURE_FLAGS_DEFAULT 4


class TISCH_SHARED Gesture: public std::vector< SmartPtr<FeatureBase> > {

	friend TISCH_SHARED std::istream& operator>> ( std::istream& s, Gesture& g );
	friend TISCH_SHARED std::ostream& operator<< ( std::ostream& s, Gesture& g );

	public:

		Gesture( ) { }
		Gesture( std::string _name, int _flags = 0 ): m_name(_name), m_flags(_flags) { }

		Gesture( const Gesture& g );
		Gesture& operator=( const Gesture& g );

		void load( InputState& state );
		void check();

		int next();  // return 0 if no further matches
		int match();

		const std::string& name() const { return m_name; }
		const int flags() const { return m_flags; }

	protected:

		void clone( const Gesture& g );

		std::string m_name;
		int m_flags;
};

#endif // _GESTURE_H_

