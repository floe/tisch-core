/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Region.h"
#include "Factory.h"
#include <algorithm>
#include <stdexcept>


std::istream& operator>> ( std::istream& s, Region& r ) {

	r.clear();
	r.gestures.clear();

	s >> r.m_flags;

	int count = 0;
	s >> count;

	while (count > 0) {
		Vector tmp;
		s >> tmp;
		r.push_back(tmp);
		count--;
	}

	count = 0;
	s >> count;

	while (count > 0) {
		Gesture tmp;
		s >> tmp;
		r.gestures.push_back(tmp);
		count--;
	}

	return s;
}

std::ostream& operator<< ( std::ostream& s, Region& r ) {

	s << r.m_flags << " ";

	s << r.size() << " ";
	std::vector<Vector>::iterator pos = r.begin();
	std::vector<Vector>::iterator end = r.end();
	for ( ; pos != end; pos++ ) s << *pos << " ";

	s << r.gestures.size() << " ";
	std::vector<Gesture>::iterator gpos = r.gestures.begin();
	std::vector<Gesture>::iterator gend = r.gestures.end();
	for ( ; gpos != gend; gpos++ ) s << *gpos << " ";

	return s;
}


Region::Region( unsigned int _flags ): std::vector<Vector>(), gestures(), m_flags(_flags) { }

int Region::contains( Vector v ) {

	std::vector<Vector>::iterator pos1 = this->begin();
	std::vector<Vector>::iterator pos2 = pos1; pos2++;
	std::vector<Vector>::iterator end  = this->end();

	Vector v1,v2;
	double tmp;
	int lcount = 0;
	//int rcount = 0;

	while ( pos1 != end ) {

		v1 = *pos1; pos1++;
		v2 = *pos2; pos2++;

		if ( pos2 == end ) pos2 = this->begin();

		if ((v1.y >  v.y) && (v2.y >  v.y)) continue;
		if ((v1.y <= v.y) && (v2.y <= v.y)) continue;

		tmp = v1.x + ( (v.y - v1.y) / (v2.y - v1.y) ) * (v2.x - v1.x);

		if (tmp < v.x) lcount++;
		//if (tmp > x) rcount++;
	}

	return (lcount % 2);
}


StateRegion::StateRegion( unsigned long long _id ): Region(), id(_id), first(1), state() { }


StateRegion& StateRegion::operator=( Region& reg ) {
	if (this == &reg) return *this;
	*((Region*)this) = reg;
	return *this;
}

void StateRegion::update() {

	gst = gestures.begin();
	first = 1;

	for (std::vector<Gesture>::iterator loadgst = gst; loadgst != gestures.end(); loadgst++)
		loadgst->load( state );
}


Gesture* StateRegion::nextMatch() {

	while (gst != gestures.end()) {

		if ((!first) && (!gst->next())) {
			gst++;
			first = 1;
			continue;
		}

		first = 0;

		if (gst->match()) 
			return &(*gst);
	}

	gst = gestures.begin();
	first = 1;

	return 0;
}

