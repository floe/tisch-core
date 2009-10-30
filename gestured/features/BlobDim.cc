/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BlobDim.h"
#include "Factory.h"


std::ostream& operator<<( std::ostream& s, Dimensions& d ) {
	s << d.x1 << " " << d.y1 << " ";
	s << d.x2 << " " << d.y2 << " ";
	s << d.size;
	return s;
}

std::istream& operator>>( std::istream& s, Dimensions& d ) {
	s >> d.x1 >> d.y1;
	s >> d.x2 >> d.y2;
	s >> d.size;
	return s;
}



BlobDim::BlobDim( int tf ): Feature<Dimensions>( tf ) { }
BlobDim::~BlobDim() { }

int BlobDim::check() {
	if (m_bounds.size() != 2) return 1;
	Dimensions lower = m_bounds[0];
	Dimensions upper = m_bounds[1];
	if ((cur->x1   < lower.x1  ) || (cur->x1   > upper.x1  )) return 0;
	if ((cur->x2   < lower.x2  ) || (cur->x2   > upper.x2  )) return 0;
	if ((cur->y1   < lower.y1  ) || (cur->y1   > upper.y1  )) return 0;
	if ((cur->y2   < lower.y2  ) || (cur->y2   > upper.y2  )) return 0;
	if ((cur->size < lower.size) || (cur->size > upper.size)) return 0;
	return 1;
}

void BlobDim::load( InputState& state ) {

	ids.clear();

	for (int i = 0; i < INPUT_TYPE_COUNT; i++) {

		if (!(typeflags & (1<<i))) continue;

		std::map<int,BlobHistory>::iterator pos = state[i].begin(); 
		std::map<int,BlobHistory>::iterator end = state[i].end();

		while (pos != end) {
			Dimensions tmp;
			tmp.x1 = pos->second[0].axis1.x;
			tmp.y1 = pos->second[0].axis1.y;
			tmp.x2 = pos->second[0].axis2.x;
			tmp.y2 = pos->second[0].axis2.y;
			tmp.size = pos->second[0].size;
			ids.push_back(tmp);//pos->second.pos);
			pos++;
		}
	}

	cur = ids.begin();
	has_result = 0;

	if ((cur != ids.end()) && check()) {
		m_result = *cur;
		has_result = 1;
	}
}

int BlobDim::next() {
	if (cur == ids.end()) return 0;
	if (++cur != ids.end()) {
		if (check()) {
			m_result = *cur;
			has_result = 1;
		} else has_result = 0;
		return 1;
	}
	has_result = 0;
	return 0;
}

RegisterFeature( BlobDim );

