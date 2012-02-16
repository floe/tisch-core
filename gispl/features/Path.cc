/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Path.h"
#include "Factory.h"

Path::Path( unsigned int tf ): Feature<Vector>( tf ) { }
Path::~Path() { }

// The following code is inspired by the $1 gesture recognizer by J.Wobbrock, A.Wilson and Y.Li.
// Original code is under New BSD License; since this is a rewrite using
// the libTISCH geometry classes, LGPL relicensing should be acceptable.

double path_length( std::vector<Vector>& path ) {
	double length = 0;
	for (unsigned int i = 1; i < path.size(); i++)
		length += (path[i] - path[i-1]).length();
	return length;
}

double path_dist( std::vector<Vector>& path1, std::vector<Vector>& path2 ) {
	double dist = 0;
	int n = path1.size();
	for (int i = 0; i < n; i++)
		dist += (path1[i] - path2[i]).length();
	return dist/(double)n;
}

Vector bbox_dim( std::vector<Vector>& path ) {

	double minx = __DBL_MIN__;
	double miny = __DBL_MIN__; 
	double minz = __DBL_MIN__;
	double maxx = __DBL_MAX__; 
	double maxy = __DBL_MAX__; 
	double maxz = __DBL_MAX__;

	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++) {

		if (p->x < minx) minx = p->x; 
		if (p->y < miny) miny = p->y; 
		if (p->z < minz) minz = p->z;

		if (p->x > maxx) maxx = p->x; 
		if (p->y > maxy) maxy = p->y; 
		if (p->z > maxz) maxz = p->z;
	}

	return Vector( maxx-minx, maxy-miny, maxz-minz );
}

Vector centroid( std::vector<Vector>& path ) {
	Vector result(0,0,0);
	double n = path.size();
	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++)
		result = result + *p;
	return result*(1.0/n);
}


void Path::load( InputState& state ) {

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


RegisterFeature( Path );

