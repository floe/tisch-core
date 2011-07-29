/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MASTERCONTAINER_H_
#define _MASTERCONTAINER_H_

#include "Container.h"
#include <Matcher.h>

class InputThread;
class InternalMatcher;


class TISCH_SHARED MasterContainer: public Container {

	public:

		 MasterContainer( int w, int h, const char* target = "127.0.0.1" );
		~MasterContainer();

		void doUpdate( Widget* target = 0 );
		void adjust( int w, int h );
		void usePeak();
		int process_gestures();

	protected:

		InternalMatcher* matcher;
		MatcherTUIOInput input;
		InputThread* inthread;

};

#endif // _MASTERCONTAINER_H_

