/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BlobGroup.h"
#include "Factory.h"


BlobGroup::BlobGroup( unsigned int tf ): Feature<Vector>( tf ) { }
BlobGroup::~BlobGroup() { }


void BlobGroup::load( InputState& state ) {

	groups.clear();
	unsigned int count = (unsigned int)m_bounds[0].x;
	double radius = m_bounds[0].y;

	// TODO: add parent ID check
	for (int t = 0; t < INPUT_TYPE_COUNT; t++) {

		if (!(typeflags & (1<<t))) continue;

		std::map<int,BlobHistory>::iterator pos = state[t].begin(); 
		std::map<int,BlobHistory>::iterator end = state[t].end();

		while (pos != end) {
			
			std::vector<Vector> cand;
			std::map<int,BlobHistory>::iterator p2 = pos; //state[t].begin();

			// check distance pairs with all other points 
			while (p2 != end) {
				if ((pos->second.pos - p2->second.pos).length() < 2*radius)
					cand.push_back( p2->second.pos );
				p2++;
			}

			// enough points in candidate group -> calculate centroid, add to group list
			if (cand.size() >= count) {
				Vector tmp;
				for (std::vector<Vector>::iterator vec = cand.begin(); vec != cand.end(); vec++)
					tmp = tmp + *vec;
				tmp = tmp * (1.0/cand.size());
				groups.push_back( tmp );
			}

			pos++;
		}
	}

	cur = groups.begin();
	has_result = 0;

	if (cur != groups.end()) {
		m_result = *cur;
		has_result = 1;
	}
}


int BlobGroup::next() {
	if (cur == groups.end()) return 0;
	if (++cur != groups.end()) {
		m_result = *cur;
		has_result = 1;
		return 1;
	}
	has_result = 0;
	return 0;
}

RegisterFeature( BlobGroup );

