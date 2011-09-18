/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BlobParent.h"
#include "Factory.h"


BlobParent::BlobParent( unsigned int tf ): Feature<int>( tf ) { }
BlobParent::~BlobParent() { }

void BlobParent::load( InputState& state ) {

	ids.clear();

	for (int i = 0; i < INPUT_TYPE_COUNT; i++) {

		if (!(typeflags & (1<<i))) continue;

		std::map<int,BlobHistory>::iterator pos = state[i].begin(); 
		std::map<int,BlobHistory>::iterator end = state[i].end();

		while (pos != end) {
			ids.push_back(pos->second.first.pid);
			pos++;
		}
	}

	cur = ids.begin();
	has_result = 0;

	if (cur != ids.end()) {
		m_result = *cur;
		has_result = 1;
	}
}


int BlobParent::next() {
	if (cur == ids.end()) return 0;
	if (++cur != ids.end()) {
		m_result = *cur;
		has_result = 1;
		return 1;
	}
	has_result = 0;
	return 0;
}

RegisterFeature( BlobParent );

