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


Window::Window( int w, int h, std::string title, int use_mouse , const char* target ):
	GLUTWindow( w, h, title ),
	MasterContainer( w, h, target )
{
	mymouse = use_mouse ? new Mouse() : NULL;

	// FIXME: hardcoded image size
	try { ghost = new IntensityImage( 640, 480, 0xF10E, 0 ); } catch (...) { ghost = NULL; }

	// default background: white
	mycolor[0] = 1.0;
	mycolor[1] = 1.0;
	mycolor[2] = 1.0;
}

Window::~Window() { }


void Window::keyboard( int key, int x, int y ) {
	if (key == 'q') { signOff(); exit(0); }
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
	if (ghost) {
		glDisable( GL_DEPTH_TEST );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glColor4f( 1.0, 1.0, 1.0, 0.15 );
		show( *ghost, 0, 0 );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
		glEnable( GL_DEPTH_TEST );
	}
	swap();
}

void Window::idle() {
	if (!process()) {
		if (mymouse) mymouse->send_blobs( width, height );
		glutPostRedisplay();
	}
}

