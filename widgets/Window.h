/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <UDPSocket.h>
#include <GLUTWindow.h>

#include "MasterContainer.h"


class TISCH_SHARED Window: public GLUTWindow, public MasterContainer {

	public:

		 Window( int w, int h, std::string title, int use_mouse = 0 );
		~Window();

		void idle();
		void display();
		void reshape( int w, int h );
		void keyboard( int key, int x, int y );

		void mouse( int num, int button, int state, int x, int y );
		void motion( int num, int x, int y );
		void passive( int num, int x, int y );
		void entry( int num, int state );

	private:

		UDPSocket output;

		void send_blobs();

		std::map<int,BasicBlob> blobs;
		int framenum;
};

#endif // _WINDOW_H_

