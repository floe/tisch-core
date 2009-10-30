/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <tisch.h>

#include "Window.h"

#define WHEEL_UP   3
#define WHEEL_DOWN 4

// unsigned long long us1;


Window::Window( int w, int h, std::string title, int use_mouse ):
	GLUTWindow( w, h, title ),
	MasterContainer( w, h ),
	output( INADDR_ANY, 0 )
{
	output.target( INADDR_LOOPBACK, TISCH_PORT_CALIB );
	//output.target( "192.168.1.120", TISCH_PORT_CALIB );

	// TODO: must always init to 0 on iPhone 
	framenum = (use_mouse ? 0 : -1);

	// default background: white
	mycolor[0] = 1.0;
	mycolor[1] = 1.0;
	mycolor[2] = 1.0;
}

Window::~Window() { }


void Window::send_blobs() {

	if (framenum == -1) return;

	for (std::map<int,BasicBlob>::iterator blob = blobs.begin(); blob != blobs.end(); blob++) {

		blob->second.peak = blob->second.pos;

		if (blob->second.value >= 1) {
			blob->second.id  = blob->first+1;
			blob->second.pid = 0;
			output << "shadow " << blob->second << std::endl;
		}

		if (blob->second.value >= 2) {
			blob->second.id  = blob->first;
			blob->second.pid = blob->first+1;
			output << "finger " << blob->second << std::endl;
		}

	}

	output << "frame " << framenum++ << std::endl;
}


void Window::keyboard( int key, int x, int y ) {
	// rather brutal, but works as Window instances should never be static
	if (key == 'q') { /*delete this;*/signOff(); exit(1); }
	if (key == 'f') glutFullScreen();
}

void Window::entry( int num, int state ) {
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

void Window::motion( int num, int x, int y ) {
	BasicBlob& foo = blobs[num];
	foo.pos.x = x;
	foo.pos.y = height-y;
	send_blobs();
}

void Window::passive( int num, int x, int y ) {
	// TODO: BasicBlob should just have a sensible default constructor
	if (blobs.find(num) == blobs.end()) entry( num, GLUT_ENTERED );
	motion( num, x, y );
}

void Window::mouse( int num, int button, int state, int x, int y ) {

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

	/*struct timeval tv; gettimeofday(&tv,0);
	us1 = tv.tv_usec + tv.tv_sec * 1000000;*/

	motion( num, x, y );
}


void Window::reshape( int _w, int _h ) {

	GLUTWindow::reshape( _w, _h );
	mode2D();

	adjust( width, height );
	doUpdate();
}


void Window::display() {
	mode2D();
	glClear(GL_DEPTH_BUFFER_BIT);
	draw();
	swap();
}

void Window::idle() {
	if (!process()) {
		send_blobs();
		glutPostRedisplay();
	}
}

