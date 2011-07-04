/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <tisch.h>

#include "MasterContainer.h"

TISCH_SHARED GLdouble g_proj[16];
TISCH_SHARED GLint    g_view[4];

TISCH_SHARED std::set<Widget*> g_widgets;

TISCH_SHARED Matcher* g_matcher;


// wrapper thread for input socket
struct TISCH_SHARED InputThread: public Thread {
	InputThread( MatcherTUIOInput* _socket ): Thread(), socket(_socket) { }
	virtual void* run() { socket->run(); return NULL; }
	MatcherTUIOInput* socket;
};


MasterContainer::MasterContainer( int w, int h, const char* target ):
	Container( w, h, w/2, h/2 ),
	matcher(), input( &matcher ), inthread( new InputThread(&input) )
{
	//region.flags( (1<<INPUT_TYPE_COUNT)-1 );
	//region.gestures.clear();
	g_matcher = &matcher;
	matcher.load_defaults();
	inthread->start();
}

MasterContainer::~MasterContainer() {
	delete inthread;
}

void MasterContainer::usePeak() {
	matcher.peakmode( 1 );
}


void MasterContainer::doUpdate( Widget* target ) {

	glGetDoublev( GL_PROJECTION_MATRIX, g_proj );
	glGetIntegerv( GL_VIEWPORT, g_view );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	Container::doUpdate( target );

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


// matcher class for use with socket thread and OpenGL mainloop
InternalMatcher::InternalMatcher(): Matcher(1) { do_run = 0; }

// called from socket context
void InternalMatcher::process_gestures() { do_run = 1; }

// called from OpenGL context
int InternalMatcher::do_process_gestures() {
	if (do_run) {
		Matcher::process_gestures();
		//std::cout << "calling process_gestures" << std::endl;
	}
	int res = do_run;
	do_run = 0;
	return res;
}
	
// called from OpenGL context (via do_process_gestures)
void InternalMatcher::request_update( int id ) { 
	std::set<Widget*>::iterator target = g_widgets.find( (Widget*)id );
	if (target == g_widgets.end()) return;
	(*target)->update();
}

// called from OpenGL context (via do_process_gestures)
void InternalMatcher::trigger_gesture( int id, Gesture* g ) {
	// deliver to widget
	std::set<Widget*>::iterator target = g_widgets.find( (Widget*)id );
	if (target == g_widgets.end()) return;
	(*target)->action( g );
}

