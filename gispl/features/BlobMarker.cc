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
	int markerID = 0;
	for (int i = 0; i < INPUT_TYPE_COUNT; i++) { 
		if (!(typeflags & (1<<i))) continue;
		
		std::map<int,BlobHistory>::iterator pos = state[i].begin(); 
		std::map<int,BlobHistory>::iterator end = state[i].end();

		while (pos != end) {
			markerID = pos->second.first.assignedMarker.markerID;
			pos++;
		}

	}

	m_result = markerID;
}

RegisterFeature( BlobMarker );

