/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
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
	output( target, TISCH_PORT_CALIB ),
	blobs(), framenum( 0 )
{ }

Mouse::~Mouse() { }


void Mouse::send_blobs() {

	std::string prefix = "ptr";
	output.start();
	output.setPrefix( prefix );

	for (std::map<int,BasicBlob>::iterator blob = blobs.begin(); blob != blobs.end(); blob++)
		output << blob->second;

	output.send( );
}


void Mouse::entry( int num, int state ) {

	if (state == GLUT_LEFT) blobs.erase( num );

	if (state == GLUT_ENTERED) {
		BasicBlob foo;
		foo.size  = 1;
		foo.value = 1;
		foo.axis1 = Vector(2,0);
		foo.axis2 = Vector(0,1);
		blobs[num] = foo;
	}
}

void Mouse::motion( int num, int x, int y ) {
	BasicBlob& foo = blobs[num];
	foo.pos.x = x;
	foo.pos.y = y; //height-y;
	send_blobs();
}

void Mouse::passive( int num, int x, int y ) {
	// TODO: BasicBlob should just have a sensible default constructor
	if (blobs.find(num) == blobs.end()) entry( num, GLUT_ENTERED );
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
	if (state == GLUT_DOWN) blobs[num].value = 2;
	if (state == GLUT_UP  ) blobs[num].value = 1;

	#ifdef TISCH_LATENCY
		if (state == GLUT_DOWN) {
			struct timeval tv; gettimeofday(&tv,0);
			us1 = tv.tv_usec + tv.tv_sec * 1000000;
		}
	#endif

	motion( num, x, y );
}

