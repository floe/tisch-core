/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Gesture.h"
#include "Factory.h"

#include <sstream>
#include <string.h>

typedef std::map<std::string,Gesture> DefaultMap;

DefaultMap& g_defaults() {
	static DefaultMap* s_defaults = new DefaultMap();
	return *s_defaults;
}


// stream operators
std::istream& operator>> ( std::istream& s, Gesture& g ) {

	char name[1024];
	if (s.peek() != '{') throw std::runtime_error("{"); s.ignore(1); 
	s >> std::ws; s.getline(name,sizeof(name),':'); if (std::string("\"name\"") != name) throw std::runtime_error(name);
	s >> std::ws; s.getline(name,sizeof(name),','); g.m_name.assign(name+1,strlen(name)-2);
	s >> std::ws; s.getline(name,sizeof(name),':'); if (std::string("\"flags\"") != name) throw std::runtime_error(name);
	s >> g.m_flags; s.ignore(1);
	s >> std::ws; s.getline(name,sizeof(name),':'); if (std::string("\"features\"") != name) throw std::runtime_error(name); s.ignore(1);
	
	// an empty gesture doesn't really make sense, so
	// let's see if we can find a default definition..
	if (s.peek() == ']') {
		DefaultMap::iterator it = g_defaults().find( g.m_name );
		if (it != g_defaults().end()) {
			g = it->second;
			s.ignore(2);
			return s;
		}
	}

	// read features
	while (s.peek() != ']') {
		std::string fname;
		s >> std::ws; s.ignore(1);
		s >> std::ws; s.getline(name,sizeof(name),':'); if (std::string("\"type\"") != name) throw std::runtime_error(std::string("\\")+name+std::string("\\"));
		s >> std::ws; s.getline(name,sizeof(name),','); fname.assign(name+1,strlen(name)-2);
		FeatureBase* res = createFeature( fname );
		if (res) {
			res->unserialize( s );
			g.push_back( res );
		} 
		s >> std::ws;
		if (s.peek() == ',') s.ignore(1);
	}
	
	// if this gesture is a default definition, store it
	if (g.m_flags & GESTURE_FLAGS_DEFAULT) {
		g.m_flags &= ~GESTURE_FLAGS_DEFAULT;
		g_defaults()[ g.m_name ] = g;
	}

	return s;
}

std::ostream& operator<< ( std::ostream& s, Gesture& g ) {
	s << "{\n\"name\":\"" << g.m_name << "\",\n";
	s << "\"flags\":" << g.m_flags << ",\n";
	s << "\"features\":[\n";
	//std::vector< SmartPtr<FeatureBase> >::iterator pos = g.begin();
	//std::vector< SmartPtr<FeatureBase> >::iterator end = g.end();
	int size = g.size();
	for (int i = 0; i < size; i++) { 
		g[i]->serialize( s );
		if (i < size-1) s << ",";
	}
	s << "]}\n";
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

