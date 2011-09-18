/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "RelativeAxisScale.h"
#include "Factory.h"

RelativeAxisScale::RelativeAxisScale( unsigned int tf ): Scale( tf ) { }
RelativeAxisScale::~RelativeAxisScale() { }

void RelativeAxisScale::load( InputState& state ) {

	m_result = 0.0;
	has_result = 0;
	int count = 0;

	for (int i = 0; i < INPUT_TYPE_COUNT; i++) {
		
		if (!(typeflags & (1<<i))) continue;

		BlobState& bs = state[i];
		int size = bs.size();
		if (size == 0) continue;

		for (std::map<int,BlobHistory>::iterator hist = bs.begin(); hist != bs.end(); hist++ ) {
			if (hist->second.size() <= 1) continue;
			double axis_old_length = hist->second[1].axis1.length();
			double axis_new_length = hist->second[0].axis1.length();
			double reldist = axis_new_length/axis_old_length;
			m_result = m_result + reldist;
			count++;
		}
	}

	if (count == 0) return;
	m_result = m_result / (double)count;
	if ((m_result == 0.0) || (m_result == 1.0)) return;
	has_result = (m_bounds.size() == 2) ? ((m_result >= m_bounds[0]) && (m_result <= m_bounds[1])) : 1;

}

RegisterFeature( RelativeAxisScale );
