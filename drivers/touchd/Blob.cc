/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <assert.h>
#include <exception>
#include <algorithm>

#include "Blob.h"
#include "Line.h"
#include "Circle.h"


// create a blob starting at point seed in image. throws std::exception when below minsize.
Blob::Blob( IntensityImage* image, Point seed, unsigned char _value, int gid, int minsize, int maxsize ) {

	id = gid;
	value = _value;
	tracked = pid = 0;

	// integrate the spot and fill it with the current counter value
	size = (int)image->integrate( seed, pos, axis1, axis2, 255, value );

	// if the spot is too small, wipe it out and abort
	if ( (size < minsize) || ((maxsize != 0) && (size > maxsize)) ){
		image->integrate( seed, pos, axis1, axis2, value, 0 );
		throw std::exception();
	}

	speed.set( 0.0, 0.0 );
	peak.set( image->getWidth()/2.0, image->getHeight()/2.0 );

	// setting variables for Configurator
	toggle = 0;
	countOfOptions = 0; // number of variables that can be manipulated
}


// scan a circular area around the blob position for non-zero pixels
unsigned char Blob::scan( IntensityImage* image, double factor ) {

	int maxrad = (int)(sqrt((double)size)*factor);

	for (int radius = 2; radius <= maxrad; radius += 2) {

		// some sanity checks
		if ( ((pos.x - radius) < 0) || ((pos.x + radius) >= image->getWidth ()) ) return 0;
		if ( ((pos.y - radius) < 0) || ((pos.y + radius) >= image->getHeight()) ) return 0;

		// scan concentric circle
		CirclePoints c( Point((int)pos.x,(int)pos.y), radius );

		for (std::vector<Point>::iterator it = c.points.begin(); it != c.points.end(); it++) {
			unsigned char tmp = image->getPixel( it->x, it->y );
			if (tmp) return tmp;
		}
	}

	return 0;
}


// use old peak, centroid, major and minor axis to find peak of blob
void Blob::setPeak( IntensityImage* image, double factor, double peakmode ) {

	if (peakmode == 0.0) { peak = pos; return; }

	int width  = image->getWidth();
	int height = image->getHeight();

	// start points along the minor axis
	Point start1 = (pos - axis2*factor).round();
	Point start2 = (pos + axis2*factor).round();

	Vector peaks[2];
	Point offs[2];

	offs[0] =  (axis1*factor).round(); peaks[0] = pos;
	offs[1] = -(axis1*factor).round(); peaks[1] = pos;

	// find the two peaks
	for (int side = 0; side < 2; side++) {

		// starting points along shifted major axis
		LinePoints lp1( start1, start1+offs[side] );
		LinePoints lp2( start2, start2+offs[side] );
		assert( lp1.points.size() == lp2.points.size() );
		double dist = 0;

		int pass = 1;
		int inc  = 5;

		// scan along shifted minor axis in 5-pixel steps until empty line found
		for (unsigned int i = 0; i < lp1.points.size(); i+=inc) {

			// new line from left to right border
			LinePoints lp3( lp1.points[i], lp2.points[i] );
			int empty = 1;

			// scan along line
			for (std::vector<Point>::iterator it = lp3.points.begin(); it != lp3.points.end(); it++) {

				// sanity check
				if ((it->x < 0) || (it->x >=  width)) continue;
				if ((it->y < 0) || (it->y >= height)) continue;

				// find pixel that is farthest from centroid
				if (image->getPixel( it->x, it->y ) == value) {
					empty = 0;
					Vector p( it->x, it->y );
					double newdist = (pos - p).length();
					if (newdist > dist) { dist = newdist; peaks[side] = p; }
				}
			}

			if (empty) {
				if (pass == 1) { i-=inc; inc=1; pass=2; }
				else if (pass == 2) break;
			}
		}
	}

	double d0,d1,dx,dy;

	// peakmode < 0: take the topmost peak, otherwise:
	// blob is smaller than threshold: take the peak farther away from the border
	// blob is larger than threshold: take the peak closest to the old one
	if (peakmode < 0) {
		d0 = peaks[0].y;
		d1 = peaks[1].y;
	} else if (size < peakmode) {
		dx = std::min( peaks[0].x,  width-peaks[0].x );
		dy = std::min( peaks[0].y, height-peaks[0].y );
		d1 = std::min( dx, dy );
		dx = std::min( peaks[1].x,  width-peaks[1].x );
		dy = std::min( peaks[1].y, height-peaks[1].y );
		d0 = std::min( dy, dx );
	} else { 
		d0 = (peaks[0]-peak).length();
		d1 = (peaks[1]-peak).length();
	}

	if (d0 < d1) peak = peaks[0]; else peak = peaks[1];
}

