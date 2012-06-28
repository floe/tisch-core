/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BlobMarker.h"
#include "Factory.h"


BlobMarker::BlobMarker( unsigned int tf ): Feature<int>( tf ) { }
BlobMarker::~BlobMarker() { }

void BlobMarker::load( InputState& state ) {

	ids.clear();

	for (int i = 0; i < INPUT_TYPE_COUNT; i++) { 
		if (!(typeflags & (1<<i))) continue;
		
		std::map<int,BlobHistory>::iterator pos = state[i].begin(); 
		std::map<int,BlobHistory>::iterator end = state[i].end();

		while (pos != end) {
			if(pos->second[0].assignedMarker.markerID != 0)
				ids.push_back(pos->second[0].assignedMarker.markerID);
			pos++;
		}

	}
	cur = ids.begin();
	if (cur != ids.end()) {
		m_result = *cur;
		has_result = 1;
	} else has_result = 0;
}

RegisterFeature( BlobMarker );

