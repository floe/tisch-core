/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>

#include "Line.h"

Line::Line( Point p1, Point p2 ) {

	x = p1.x; dx = abs(p2.x - x);
	y = p1.y; dy = abs(p2.y - y);

	if (dx >= dy) { num = dx+1; d1 = 2*dy; d2 = 2*(dy-dx); d = d1 - dx; x1 = 1; x2 = 1; y1 = 0; y2 = 1; }
					 else { num = dy+1; d1 = 2*dx; d2 = 2*(dx-dy); d = d1 - dy; x1 = 0; x2 = 1; y1 = 1; y2 = 1; }

	if (x > p2.x) { x1 = -x1; x2 = -x2; }
	if (y > p2.y) { y1 = -y1; y2 = -y2; }
}

Line::~Line() { }

void Line::follow() {
	for (int i = 0; i < num; i++) {
		foreach( x, y );
		if (d < 0) { d += d1; x += x1; y += y1; }
					else { d += d2; x += x2; y += y2; }
	}
}


LinePoints::LinePoints( Point _p1, Point _p2 ): Line( _p1, _p2 ), points() { follow(); }
LinePoints::~LinePoints() { }

void LinePoints::foreach( int x, int y ) { points.push_back( Point(x,y) ); }

