/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
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


typedef std::pair<Widget*,Gesture> Action;
typedef std::deque<Action> ActionQueue;

// matcher class for use with socket thread and OpenGL mainloop
struct TISCH_SHARED InternalMatcher: public Matcher {

	InternalMatcher(): Matcher(), queue() { do_run = 0; }

	// called from socket context
	void process_gestures() { do_run = 1; }

	// called from OpenGL context
	int do_process_gestures() {
		if (do_run) {
			Matcher::process_gestures();
			while (!queue.empty()) {
				Action& a = queue.front();
				a.first->action( &a.second );
				queue.pop_front();
			}
		}
		int res = do_run;
		do_run = 0;
		return res;
	}
		
	// called from OpenGL context (via do_process_gestures)
	void request_update( unsigned long long id ) { 
		std::set<Widget*>::iterator target = g_widgets.find( (Widget*)id );
		if (target == g_widgets.end()) return;
		(*target)->update();
	}

	// called from OpenGL context (via do_process_gestures)
	void trigger_gesture( unsigned long long id, Gesture* g ) {
		// deliver to widget
		std::set<Widget*>::iterator target = g_widgets.find( (Widget*)id );
		if (target == g_widgets.end()) return;
		//(*target)->action( g );
		queue.push_back( Action( *target, *g ) );
	}

	ActionQueue queue;

};


MasterContainer::MasterContainer( int w, int h, int defaults ):
	Container( w, h, w/2, h/2 ),
	matcher( new InternalMatcher() ), input( matcher ), inthread( new InputThread(&input) )
{
	g_matcher = matcher;
	matcher->load_defaults(defaults);
	inthread->start();
}

MasterContainer::~MasterContainer() {
	delete inthread;
	delete matcher;
}

void MasterContainer::usePeak() {
	matcher->peakmode( 1 );
}

int MasterContainer::process_gestures() {
	return matcher->do_process_gestures();
}


void MasterContainer::doUpdate( Widget* target ) {

	//std::cout << "MC::doUpdate " << (unsigned long long) target << std::endl;
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



