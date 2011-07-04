/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MASTERCONTAINER_H_
#define _MASTERCONTAINER_H_

#include <Matcher.h>
#include "Container.h"


class InputThread;

struct TISCH_SHARED InternalMatcher: public Matcher {

		InternalMatcher();

		void process_gestures();
		int do_process_gestures();

		void request_update( int id );
		void trigger_gesture( int id, Gesture* g );
};


class TISCH_SHARED MasterContainer: public Container {

	public:

		 MasterContainer( int w, int h, const char* target = "127.0.0.1" );
		~MasterContainer();

		void doUpdate( Widget* target = 0 );
		void adjust( int w, int h );
		void usePeak();

	protected:

		InternalMatcher matcher;
		MatcherTUIOInput input;
		InputThread* inthread;

};

#endif // _MASTERCONTAINER_H_

