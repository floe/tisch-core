/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <tisch.h>

#include <GLUTWindow.h>

#include "Mouse.h"

#define WHEEL_UP   3
#define WHEEL_DOWN 4


// hidden global variable for latency measurements
unsigned long long us1;


Mouse::Mouse( const char* target ):
	output( TISCH_TUIO2, target, TISCH_PORT_CALIB ),
	blobs(), framenum( 0 )
{ }

Mouse::~Mouse() { }


void Mouse::send_blobs( double w, double h ) {

	output.start();

	std::map<int,BasicBlob>::iterator blob = blobs.begin();

	#ifdef _MSC_VER
		// on windows, the first touch point is reported twice
		// (once as mouse pointer with ID -1, once as touch)
		// => skip the very first one
		if (blobs.size() > 1) blob++;
	#endif

	for ( ; blob != blobs.end(); blob++) {

		BasicBlob tmp = blob->second;

		tmp.pos.x = tmp.pos.x / w;
		tmp.pos.y = tmp.pos.y / h;

		if (tmp.type == INPUT_TYPE_FINGER) tmp.id++;
		tmp.peak = tmp.pos;

		output << tmp;
	}

	output.send( );
}


void Mouse::entry( int num, int state ) {
	if (state == GLUT_LEFT) blobs.erase( num );
	else blobs[num].type = INPUT_TYPE_HAND;
}

void Mouse::motion( int num, int x, int y ) {
	BasicBlob& foo = blobs[num];
	foo.id  = 2*num;
	foo.pid = 0;
	foo.pos.x = x;
	foo.pos.y = y;
}

void Mouse::passive( int num, int x, int y ) {
	motion( num, x, y );
}

void Mouse::button( int num, int button, int state, int x, int y ) {

	// TODO: get rid of ugly hack to support multi-wheel mice
	if (button >= WHEEL_DOWN) button = ((button-1)%2)+3;

	if ((button == WHEEL_UP) || (button == WHEEL_DOWN)) {
		if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
			// scale via ctrl + wheel
			double scale = 1.1;
			if(button == WHEEL_DOWN) scale = 0.9;
			blobs[num].axis1 = blobs[num].axis1 * scale;
			blobs[num].axis2 = blobs[num].axis2 * scale;
		} else {
			// handle rotation via wheel
			double angle = 2.0/180*M_PI;
			if (button == WHEEL_DOWN) angle = -angle;
			blobs[num].axis1.rotate( angle );
			blobs[num].axis2.rotate( angle );
		}
		return;
	}

	// handle normal mouse clicks
	if (state == GLUT_DOWN) blobs[num].type = INPUT_TYPE_FINGER;
	if (state == GLUT_UP  ) blobs[num].type = INPUT_TYPE_HAND;

	#ifdef TISCH_LATENCY
		if (state == GLUT_DOWN) {
			struct timeval tv; gettimeofday(&tv,0);
			us1 = tv.tv_usec + tv.tv_sec * 1000000;
		}
	#endif

	motion( num, x, y );
}

