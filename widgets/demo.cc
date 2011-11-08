/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>

#include "Slider.h"
#include "Window.h"
#include "Textbox.h"
#include "Checkbox.h"
#include "Tile.h"
#include "Dial.h"
#include "Label.h"

Label* lbl;

// This class demonstrates how to react to the predefined tap gesture using Button as superclass.
class MyButton: public Button {
	public:

		MyButton( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0 ):
			Button( _w,_h,_x,_y,angle,_tex) { }

		void tap( Vector pos, int id ) { lbl->set( " tap!" ); }
		void release( ) { lbl->set( " Button" ); }

		// example code for a "close" button:
		/*Container* foo = parent;
			foo->remove(this);
			delete foo; 
			delete this;*/
};


int main( int argc, char* argv[] ) {

	std::cout << "demo - libTISCH 2.0 widget layer demo" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	// create a new window: width, height, title, use_mouse
	Window* win = new Window( 800, 480, "Widget Demo", (argc > 1) );
	win->texture( 0 );

	Container* foo = new Container( 400,300, 0, 0, M_PI*20/180, 0, 0xFF );
	win->add( foo );

	foo->add( new Label( "Container", 200,20, 0,120, 0,1 ) );

	foo->add( new MyButton( 60,30, -150,90 ) );
	foo->add( lbl = new Label( " Button", 200,20, -30,90 ) );

	foo->add( new Checkbox( 20,20, -170,50 ) );
	foo->add( new Label( " Checkbox", 200,20, -30,50 ) );

	foo->add( new Slider( 60,20, -150,20 ) );
	foo->add( new Label( " Slider", 200,20, -30,20 ) );

	foo->add( new Dial( 50, -150,-30, 3.1 ) );
	foo->add( new Label( " Dial", 200,20, -30,-30 ) );

	Textbox* txt = new Textbox( 512*25/64, 25, 272*25/64, -80,-80 );
	txt->set( "foobar" );
	foo->add( txt );
	foo->add( new Label( " Textbox", 200,20, 110,-80 ) );

	// the following objects behave like Tiles, but are
	// actually Containers so that labels can be added 
	Container* tile1 = new Container( 100,100, 230,-120, M_PI*330/180, 0, 0xFF );
	tile1->add( new Label( "Tile", 80,15, 0,0, 0,1 ) );
	win->add( tile1 );

	Container* tile2 = new Container( 100,100, 180,100, M_PI*50/180, 0, 0xFF );
	tile2->add( new Label( "Subtile", 80,15, 0,0, 0,1 ) );
	foo->add( tile2 );

	win->update();
	win->run();
}

