/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Gesture.h"
#include "Factory.h"

#include <sstream>

typedef std::map<std::string,Gesture> DefaultMap;

DefaultMap& g_defaults() {
	static DefaultMap* s_defaults = new DefaultMap();
	return *s_defaults;
}


// stream operators
std::istream& operator>> ( std::istream& s, Gesture& g ) {

	int count = 0;
	g.clear();

	s >> g.m_name >> g.m_flags >> count;

	// an empty gesture doesn't really make sense, so
	// let's see if we can find a default definition..
	if (count == 0) {
		g.check();
		return s;
	}

	// read features
	while (count > 0) {
		count--;
		std::string name; s >> name;
		FeatureBase* res = createFeature( name );
		if (!res) continue;
		res->unserialize( s );
		g.push_back( res );
	}
	
	// if this gesture is a default definition, store it
	if (g.m_flags & GESTURE_FLAGS_DEFAULT) {
		g.m_flags &= ~GESTURE_FLAGS_DEFAULT;
		g_defaults()[ g.m_name ] = g;
	}

	return s;
}

std::ostream& operator<< ( std::ostream& s, Gesture& g ) {
	s << g.m_name << " " << g.m_flags << " " << g.size() << " ";
	std::vector< SmartPtr<FeatureBase> >::iterator pos = g.begin();
	std::vector< SmartPtr<FeatureBase> >::iterator end = g.end();
	for ( ; pos != end; pos++ ) { 
		(*pos)->serialize( s );
		s << " ";
	}
	return s;
}


// destructor not necessary due to use of smart pointers

// copy constructor
Gesture::Gesture( const Gesture& g ):
	m_name ( g.m_name  ),
	m_flags( g.m_flags )
{
	clone( g );
}

// assignment operator
Gesture& Gesture::operator=( const Gesture& g ) {
	if (this == &g) return *this;
	m_name  = g.m_name;
	m_flags = g.m_flags;
	clear();
	clone( g );
	return *this;
}

// clone helper function (for copy con. & assignment op.)
void Gesture::clone( const Gesture& g ) {
	std::vector< SmartPtr<FeatureBase> >::const_iterator fpos = g.begin();
	std::vector< SmartPtr<FeatureBase> >::const_iterator fend = g.end();
	for ( ; fpos != fend; fpos++ ) 
		push_back( (*fpos)->clone() );
}


// check size & potentially replace by default definition
void Gesture::check() {
	if (size() == 0) {
		DefaultMap::iterator it = g_defaults().find( m_name );
		if (it != g_defaults().end()) {
			m_flags = it->second.m_flags;
			clone( it->second );
		}
	}
}


void Gesture::load( InputState& state ) {
	for (std::vector< SmartPtr<FeatureBase> >::iterator myft = begin(); myft != end(); myft++) 
		(*myft)->load( state );
}

int Gesture::next() {
	for (std::vector< SmartPtr<FeatureBase> >::iterator myft = begin(); myft != end(); myft++) 
		if (!(*myft)->next()) return 0;
	return size() ? 1 : 0;
}

int Gesture::match() {

	// all features must match at least once
	int nomatch = size() ? 0 : 1;

	// check all features of this gesture
	for (std::vector< SmartPtr<FeatureBase> >::iterator myft = begin(); myft != end(); myft++) 
		if (!((*myft)->has_result)) nomatch = 1;

	return !nomatch;
}

