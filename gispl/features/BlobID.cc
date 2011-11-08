/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BlobID.h"
#include "Factory.h"


BlobID::BlobID( unsigned int tf ): Feature<int>( tf ) { mode = 0; }
BlobID::~BlobID() { }

void BlobID::load( InputState& state ) {

	ids.clear();

	// check mode flag
	if (m_bounds.size() == 1) mode = m_bounds[0];

	for (int i = 0; i < INPUT_TYPE_COUNT; i++) {

		if (!(typeflags & (1<<i))) continue;

		if (mode == 0) {

			// normal mode - match all existing IDs
			std::map<int,BlobHistory>::iterator pos = state[i].begin(); 
			std::map<int,BlobHistory>::iterator end = state[i].end();

			while (pos != end) {
				ids.push_back(pos->first);
				pos++;
			}

		} else if (mode == -1) {

			// "inverted" mode - match all vanished IDs
			ids.insert(ids.begin(),state[i].lost.begin(),state[i].lost.end());

		}
	}

	cur = ids.begin();
	has_result = 0;

	if (cur != ids.end()) {
		if ((m_bounds.size() == 2) && ((*cur < m_bounds[0]) || (*cur > m_bounds[1]))) return;
		m_result = *cur;
		has_result = 1;
	}
}


int BlobID::next() {

	if (cur == ids.end()) return 0;

	if (++cur != ids.end()) {
		m_result = *cur;
		has_result = 1;
		if ((m_bounds.size() == 2) && ((*cur < m_bounds[0]) || (*cur > m_bounds[1]))) has_result = 0;
		return 1;
	}

	has_result = 0;
	return 0;
}

RegisterFeature( BlobID );

