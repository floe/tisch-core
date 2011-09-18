/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
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
	MasterContainer( w, h, use_mouse ? 1 : 0 )
{
	mymouse = use_mouse ? new Mouse() : NULL;

	// create ghost overlays from shared memory
	// FIXME: hardcoded image size
	for (int i = 0; i < 3; i++) try {
		ghost[i] = new IntensityImage( 640, 480, i+0xF10E, 0 );
	} catch (...) {
		ghost[i] = NULL;
	}

	// default background: white
	mycolor[0] = 1.0;
	mycolor[1] = 1.0;
	mycolor[2] = 1.0;
}

Window::~Window() { }


void Window::keyboard( int key, int x, int y ) {
	if (key == 'q') exit(0);
	if (key == 'f') glutFullScreen();
}

void Window::entry  ( int num, int state    ) { if (mymouse) mymouse->entry  ( num, state         ); }
void Window::motion ( int num, int x, int y ) { if (mymouse) mymouse->motion ( num, x, height - y ); }
void Window::passive( int num, int x, int y ) { if (mymouse) mymouse->passive( num, x, height - y ); }

void Window::mouse( int num, int button, int state, int x, int y ) { if (mymouse) mymouse->button( num, button, state, x, height - y ); }


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
	if (ghost[0] || ghost[1] || ghost[2]) {
		glDisable( GL_DEPTH_TEST );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		if (ghost[0]) { glColor4f( 1.0, 0.0, 0.0, 0.1 ); show( *ghost[0], 0, 0 ); }
		if (ghost[1]) { glColor4f( 0.0, 1.0, 0.0, 0.1 ); show( *ghost[1], 0, 0 ); }
		if (ghost[2]) { glColor4f( 0.0, 0.0, 1.0, 0.1 ); show( *ghost[2], 0, 0 ); }
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
		glEnable( GL_DEPTH_TEST );
	}
	swap();
}

void Window::idle() {
	if (mymouse) mymouse->send_blobs( width, height );
	if (process_gestures()) {
		glutPostRedisplay();
	}
}

