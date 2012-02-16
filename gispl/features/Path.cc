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

std::vector<Vector> rotate_path( std::vector<Vector>& path, double rad ) {

	std::vector<Vector> result;
	Vector center = centroid( path );

	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++) {
		Vector tmp = (*p - center); tmp.rotate(rad);
		result.push_back( tmp + center );
	}

	return result;
}

std::vector<Vector> scale_path( std::vector<Vector>& path, double factor ) {

	std::vector<Vector> result;
	Vector bbox = bbox_dim( path );

	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++)
		result.push_back( Vector( p->x * (factor/bbox.x), p->y * (factor/bbox.y), 0 ) ); //p->z * (factor/bbox.z) ) );

	return result;
}

std::vector<Vector> translate_path( std::vector<Vector>& path, Vector shift ) {

	std::vector<Vector> result;
	Vector center = centroid( path );

	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++)
		result.push_back( (*p - center) + shift );

	return result;
}

double direction( std::vector<Vector>& path ) {
	Vector center = centroid( path );
	return atan2( center.y - path[0].y, center.x - path[0].x );
}

std::vector<Vector> vectorize( std::vector<Vector>& path ) {

	std::vector<Vector> result;
	double mag,sum = 0.0;

	for (std::vector<Vector>::iterator p = path.begin(); p != path.end(); p++) {
		result.push_back( *p );
		sum += p->x * p->x + p->y * p->y;
	}

	mag = sqrt(sum);
	for (std::vector<Vector>::iterator r = result.begin(); r != result.end(); r++) {
		*r = *r * (1.0/mag);
	}

	return result;
}

std::vector<Vector> resample( std::vector<Vector>& path, int num_points ) {

	std::vector<Vector> result; result.push_back(path[0]);
	double interval = path_length( path ) / (num_points-1);
	double total_dist = 0.0;

	for (unsigned int i = 1; i < path.size(); i++) {
		double dist = (path[i] - path[i-1]).length();
		if ((total_dist+dist) >= interval) {
			double qx = path[i-1].x + ((interval - total_dist) / dist) * (path[i].x - path[i-1].x);
			double qy = path[i-1].y + ((interval - total_dist) / dist) * (path[i].y - path[i-1].y);
			Vector q(qx,qy,0);
			result.push_back( q );
			//path.insert( &(path[i+1]), q ); // FIXME
			total_dist = 0.0;
		} else total_dist += dist;
	}

	if ((int)result.size() == num_points-1)
		result.push_back( *(path.rbegin()) );

	return result;
}

double protractor_distance( std::vector<Vector>& path1, std::vector<Vector>& path2 ) {
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

void process_path( std::vector<Vector>& path ) {
	// resample(path,64);
	// rotate_path(path,direction);
	// scale_path(path, 250.0);
	// translate_path(path,Vector(0,0,0));
	// vectorize(path);
}




// actual feature value calculation takes place below

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

