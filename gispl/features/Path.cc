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

// piecewise length of path
double path_length( std::vector<Vector>& path ) {
	double length = 0;
	for (unsigned int i = 1; i < path.size(); i++)
		length += (path[i] - path[i-1]).length();
	return length;
}

/*double path_dist( std::vector<Vector>& path1, std::vector<Vector>& path2 ) {
	double dist = 0;
	int n = path1.size();
	for (int i = 0; i < n; i++)
		dist += (path1[i] - path2[i]).length();
	return dist/(double)n;
}*/

// determine centroid
Vector path_centroid( std::vector<Vector>& path ) {
	Vector result(0,0,0);
	double n = path.size();
	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++)
		result = result + *p;
	return result*(1.0/n);
}

// shift centroid to origin, rotate first point onto x axis, scale to cube
void path_transform( std::vector<Vector>& path, Vector center, double scale ) {

	double minx = __DBL_MAX__; double maxx = __DBL_MIN__; 
	double miny = __DBL_MAX__; double maxy = __DBL_MIN__; 
	double minz = __DBL_MAX__; double maxz = __DBL_MIN__;

	// determine angle from centroid to first point
	double direction = atan2( center.y - path[0].y, center.x - path[0].x ); // FIXME: only 2D

	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++) {
		Vector tmp = (*p - center);
		tmp.rotate(direction);
		if (tmp.x < minx) minx = tmp.x; if (tmp.x > maxx) maxx = tmp.x; 
		if (tmp.y < miny) miny = tmp.y; if (tmp.y > maxy) maxy = tmp.y; 
		if (tmp.z < minz) minz = tmp.z; if (tmp.z > maxz) maxz = tmp.z;
		*p = tmp;
	}

	Vector bbox = Vector( maxx-minx, maxy-miny, maxz-minz );
	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++)
		*p = Vector( p->x * (scale/bbox.x), p->y * (scale/bbox.y), 0 ); // FIXME: tmp.z * (scale/bbox.z) ) );*/
}

// prepare path for protractor matcher
void path_vectorize( std::vector<Vector>& path ) {

	double sum = 0.0;
	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++) 
		sum += p->x * p->x + p->y * p->y; // FIXME: + p->z * p->z?

	double mag = sqrt(sum);
	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++) 
		*p = *p * (1.0/mag);
}


std::vector<Vector> path_resample( std::vector<Vector>& path, int num_points ) {

	Vector cur = path[0];
	Vector prev = cur;
	unsigned int i = 1;

	std::vector<Vector> result; result.push_back( prev );
	double interval = path_length( path ) / (num_points-1);
	double total_dist = 0.0;

	while (i < path.size()) {

		cur = path[i];

		double dist = (cur - prev).length();

		if ((total_dist+dist) >= interval) {
			Vector q = prev + (cur - prev) * ((interval - total_dist) / dist);
			result.push_back( q );
			prev = q; 
			total_dist = 0.0;
		} else {
			total_dist += dist;
			prev = cur;
			i++;
		}
	}

	if ((int)result.size() == num_points-1)
		result.push_back( *(path.rbegin()) );

	return result;
}

double path_distance( std::vector<Vector>& path1, std::vector<Vector>& path2 ) {
	double a = 0.0;
	double b = 0.0;
	int n = path1.size();
	for (int i = 0; i < n; i++) {
		a += path1[i].x * path2[i].x + path1[i].y * path2[i].y;
		b += path1[i].x * path2[i].y - path1[i].y * path2[i].x;
	}
	double angle = atan(b/a);
	return acos( a*cos(angle) + b*sin(angle) );
}

std::vector<Vector> path_process( std::vector<Vector>& path ) {

	Vector centroid,bbox;

	std::vector<Vector> result = path_resample( path, 64 );
	path_transform( result, path_centroid(result), 250 );
	path_vectorize( result );

	return result;
}


// actual feature value calculation takes place below

void Path::load( InputState& state ) {

	if (m_bounds.size() == 0) return;

	if (path.empty())
		path = path_process( m_bounds );

	m_result = Vector(0,0,0);
	has_result = 0;
	int count = 0;

	for (int i = 0; i < INPUT_TYPE_COUNT; i++) {

		if (!(typeflags & (1<<i))) continue;

		BlobState& bs = state[i];

		for (std::map<int,BlobHistory>::iterator hist = bs.begin(); hist != bs.end(); hist++ ) {
			std::vector<Vector> rawpath; 
			for (std::deque<BasicBlob>::iterator b = hist->second.begin(); b != hist->second.end(); b++)
				rawpath.push_back( b->pos );

			std::vector<Vector> curpath = path_process( rawpath );
			m_result.x += path_distance( curpath, path );
			count++;
		}
	}

	if (count == 0) return;
	m_result = m_result * (1.0/(double)count);
	if (m_result.x >= 0.8) has_result = 1;
}


RegisterFeature( Path );

