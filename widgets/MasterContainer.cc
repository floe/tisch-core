/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <tisch.h>

#include "MasterContainer.h"

// This timeout should be shorter than 1/f of the fastest tracker.
// This is the Wiimote at 200 Hz = 5000 uS -> timeout 4500 uS.
struct timeval tv = { 0, 4500 };

TISCH_SHARED GLdouble g_proj[16];
TISCH_SHARED GLint    g_view[4];

TISCH_SHARED std::set<Widget*> g_widgets;


MasterContainer::MasterContainer( int w, int h, const char* target ):
	Container( w, h, w/2, h/2 ),
	input( INADDR_ANY, 0, &tv )
{
	std::cout << "connecting to gestured..." << std::flush;
	input.target( target, TISCH_PORT_EVENT );
	std::cout << " connection successful." << std::endl;
	//region.flags( (1<<INPUT_TYPE_COUNT)-1 );
	//region.gestures.clear();
}

MasterContainer::~MasterContainer() {}

void MasterContainer::signOff() {
	input << "bye 0" << std::endl;
}

void MasterContainer::usePeak() {
	input << "use_peak 0" << std::endl;
}


void MasterContainer::doUpdate( Widget* target, std::ostream* ost ) {

	glGetDoublev( GL_PROJECTION_MATRIX, g_proj );
	glGetIntegerv( GL_VIEWPORT, g_view );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	Container::doUpdate( target, &input );

	glPopMatrix();
}


void MasterContainer::adjust( int width, int height ) {
	
	sx = width/w;
	sy = height/h;

	x = width/2.0;
	y = height/2.0;

	// to have approximately the same aspect ratio as on
	// normal screens, switch the iPhone to landscape
	#ifdef TISCH_IPHONE
		angle = M_PI/2.0;
		sx = height/w;
		sy = width/h;
	#endif
}


int MasterContainer::process() {

	std::string type;
	unsigned long long tmp;

	input >> type >> tmp;

	if (!input || !tmp) {
		input.clear();
		return 0;
	}

	if (type.compare("update") == 0) {
		// update widget
		Widget* target = (Widget*)tmp;
		doUpdate( target );
	}

	if (type.compare("gesture") == 0) {
		// parse gesture
		Gesture gesture; input >> gesture;
		if (!input) { input.flush(); return 0; }
		// deliver to widget
		std::set<Widget*>::iterator target = g_widgets.find((Widget*)tmp);
		if (target == g_widgets.end()) return 0;
		(*target)->action(&gesture);
	}

	return 1;
}

