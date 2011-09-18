/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Motion.h"
#include "Factory.h"

Motion::Motion( unsigned int tf ): Feature<Vector>( tf ) { }
Motion::~Motion() { }

void Motion::load( InputState& state ) {

	m_result = Vector(0,0,0);
	has_result = 0;
	int count = 0;

	for (int i = 0; i < INPUT_TYPE_COUNT; i++) {

		if (!(typeflags & (1<<i))) continue;

		BlobState& bs = state[i];

		for (std::map<int,BlobHistory>::iterator hist = bs.begin(); hist != bs.end(); hist++ ) {
			if (hist->second.size() <= 1) continue;
			Vector tmp = hist->second.pos - hist->second[1].pos;
			m_result = m_result + tmp;
			count++;
		}
	}

	if (count == 0) return;
	m_result = m_result * (1.0/(double)count);

	double tmp = m_result.length();
	if (tmp == 0.0) return;

	has_result = (m_bounds.size() == 2) ? ((tmp >= m_bounds[0].length()) && (tmp <= m_bounds[1].length())) : 1;
}


RegisterFeature( Motion );

