/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <nanolibc.h>

#include "Window.h"
#include "Tile.h"
#include "Textbox.h"

// #define ADD_KBD


int main( int argc, char* argv[] ) {

	std::cout << "picview - TISCH widget layer demo 1.0 beta1" << std::endl;
	std::cout << "(c) 2009 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	Window* win = new Window( 640, 480, "PicView", mouse );
	win->texture(0);

	srandom(45890);

	// load the textures
	for (int i = mouse+1; i < argc; i++) {
		RGBATexture* tmp = new RGBATexture( argv[i] );
		Container* foo = new Container( 
			tmp->width(1)/5, 
			tmp->height(1)/5,
			(int)(((double)random()/(double)RAND_MAX)*600-300),
			(int)(((double)random()/(double)RAND_MAX)*450-225),
			(int)(((double)random()/(double)RAND_MAX)*360),
			tmp, 0xFF
		);

		#ifdef ADD_KBD
			double tbw = tmp->width(1)/15;
			foo->add( new Textbox(tbw,75*tbw/500,275*tbw/500, 0, tmp->height()/20) );
		#endif
		win->add( foo );
	}

	win->update();
	win->run();
}

