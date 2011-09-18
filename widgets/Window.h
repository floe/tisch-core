/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <Socket.h>
#include <GLUTWindow.h>

#include <Mouse.h>
#include <MasterContainer.h>


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

	// The following redeclarations from MasterContainer are necessary to have SWIG properly
	// wrap multiple inheritance in C# and Java. Thanks to Martin Schanzenbach for this trick.
	#ifdef SWIG
	
		void add( Widget* w, int back = 1 );
		void remove( Widget* widget, int unreg = 1 );
		void update( Widget* target = 0 );

		void adjust( int w, int h );

		void usePeak();

	#endif

	private:

		Mouse* mymouse;
		IntensityImage* ghost[3];
};

#endif // _WINDOW_H_

