/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BlobCount.h"
#include "Factory.h"


BlobCount::BlobCount( unsigned int tf ): Feature<int>( tf ) { }
BlobCount::~BlobCount() { }

void BlobCount::load( InputState& state ) {
	int tmp = 0;
	for (int i = 0; i < INPUT_TYPE_COUNT; i++) { 
		if (!(typeflags & (1<<i))) continue;
		tmp += state[i].size();
	}

	has_result = 1;

	if (m_bounds.size() >= 2)
		has_result = ((tmp >= m_bounds[0]) && (tmp <= m_bounds[1]));

	m_result = tmp;
}

RegisterFeature( BlobCount );

