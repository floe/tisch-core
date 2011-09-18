/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Circle.h"

Circle::Circle( Point c, int radius ) {

	cx = c.x;
	cy = c.y;

	x = 0;
	y = radius;
	d = 3 - 2*radius;

}

Circle::~Circle() { }


void Circle::follow() {

	while (x < y) {

		foreach( cx+x, cy+y );
		foreach( cx+x, cy-y );
		foreach( cx-x, cy+y );
		foreach( cx-x, cy-y );

		foreach( cx+y, cy+x );
		foreach( cx+y, cy-x );
		foreach( cx-y, cy+x );
		foreach( cx-y, cy-x );

		if (d < 0) {
			d = d + 4*x + 6;
		} else {
			d = d + 4*(x-y) + 10;
			y = y - 1;
		}

		x = x + 1;
	}
}


CirclePoints::CirclePoints( Point _c, int _radius ): Circle( _c, _radius ), points() { follow(); }
CirclePoints::~CirclePoints() { }

void CirclePoints::foreach( int x, int y ) { points.push_back( Point(x,y) ); }

