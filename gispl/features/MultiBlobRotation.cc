/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "MultiBlobRotation.h"
#include "Factory.h"

MultiBlobRotation::MultiBlobRotation( unsigned int tf ): Rotation( tf ) { }
MultiBlobRotation::~MultiBlobRotation() { }

void MultiBlobRotation::load( InputState& state ) {

	m_result = 0.0;
	has_result = 0;
	int count = 0;

	for (int i = 0; i < INPUT_TYPE_COUNT; i++) {
		
		if (!(typeflags & (1<<i))) continue;

		BlobState& bs = state[i];
		int size = bs.size();
		if (size == 0) continue;

		Vector centroid0;
		Vector centroid1;

		for (std::map<int,BlobHistory>::iterator hist = bs.begin(); hist != bs.end(); hist++ ) {
			if (hist->second.size() <= 1) continue;
			//centroid0 = centroid0 + hist->second.first.pos;
			centroid0 = centroid0 + hist->second[1].pos;
			centroid1 = centroid1 + hist->second.pos;
		}

		centroid0 = centroid0 * (1.0/(double)size);
		centroid1 = centroid1 * (1.0/(double)size);

		for (std::map<int,BlobHistory>::iterator hist = bs.begin(); hist != bs.end(); hist++ ) {

			if (hist->second.size() <= 1) continue;

			//Vector tmp1 = hist->second.first.pos - centroid0;
			Vector tmp1 = hist->second[1].pos - centroid0;
			Vector tmp2 = hist->second.pos - centroid1;

			tmp1.normalize(); tmp2.normalize();
			double angle = acos(tmp1*tmp2);
			Vector cross = tmp1 & tmp2;

			if (cross.z < 0) angle = -angle;
			m_result = m_result + angle;
			count++;
		}
	}

	if (count <= 1) return;
	m_result = m_result / (double)count;

	if (m_result == 0.0) return;
	while (m_result <  0.0) m_result += 2*M_PI;

	has_result = (m_bounds.size() == 2) ? ((m_result >= m_bounds[0]) && (m_result <= m_bounds[1])) : 1;
}


RegisterFeature( MultiBlobRotation );

