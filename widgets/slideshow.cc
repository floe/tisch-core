/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <nanolibc.h>

#include "Window.h"
#include "Label.h"
#include "Container.h"
#include <Motion.h>


class SlideContainer: public Container {

	public:

		SlideContainer( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF):
			Container( _w,_h,_x,_y,angle,_tex,mode) 
		{
			region.flags( INPUT_TYPE_ANY | REGION_FLAGS_VOLATILE );
		}

		void apply( Vector delta ) {
			x += delta.x;
			vel.x = delta.x;
			if (fabs(vel.x) < 0.1) slide = 0;
			if (x >  w/2) x =  w/2;
			if (x < -w/2) x = -w/2;
		}
};


int main( int argc, char* argv[] ) {

	std::cout << "slideshow - libTISCH 2.0 widget layer demo" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	if (argc <= mouse+1) {
		std::cout << "\nUsage: slideshow [-m] pic1.png pic2.png ..." << std::endl;
		return 1;
	}

	Window* win = new Window( 800, 480, "slideshow", mouse );
	win->texture(0);

	double total_width = 0;
	std::map<RGBATexture*,double> texlist;

	// load the textures
	for (int i = mouse+1; i < argc; i++) {
		RGBATexture* tmp = new RGBATexture( argv[i] );
		double new_width = tmp->width(1)*480/tmp->height(1);
		total_width += new_width;
		texlist[tmp] = new_width;
	}

	// create appropriately sized container
	SlideContainer* cont = new SlideContainer( total_width,480, total_width/2,0, 0,0, TISCH_TILE_MOVE | TISCH_TILE_SLIDE );
	cont->shadow = false;
	double currpos = -total_width/2;

	// create labels, add to container
	for (std::map<RGBATexture*,double>::iterator it = texlist.begin(); it != texlist.end(); it++) {
		Label* lbl = new Label( "", it->second, 480, currpos+it->second/2, 0, 0, 0, 0, it->first );
		cont->add( lbl );
		currpos += it->second;
	}

	win->add( cont );
	win->update();
	win->run();
}

