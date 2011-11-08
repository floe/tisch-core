/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "RelativeAxisRotation.h"
#include "Factory.h"

RelativeAxisRotation::RelativeAxisRotation( unsigned int tf ): Rotation( tf ) { }
RelativeAxisRotation::~RelativeAxisRotation() { }

void RelativeAxisRotation::load( InputState& state ) {

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
			Vector axis_old = hist->second[1].axis1; axis_old.normalize();
			Vector axis_new = hist->second[0].axis1; axis_new.normalize();
			Vector rot_axis = axis_old & axis_new;
			//if (rot_axis.z != 0.0) std::cout << "rotation: " << rot_axis.z << std::endl;
			m_result += (2*M_PI - rot_axis.z);
			count++;
		}
	}

	if (count == 0) return;
	m_result = m_result / (double)count;
	if ((m_result == 0.0) || (m_result == 2*M_PI)) return;
	has_result = (m_bounds.size() == 2) ? ((m_result >= m_bounds[0]) && (m_result <= m_bounds[1])) : 1;
}


RegisterFeature( RelativeAxisRotation );

