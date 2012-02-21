/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2012 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <sstream>

#include "Slider.h"
#include "Window.h"
#include "Textbox.h"
#include "Checkbox.h"
#include "Tile.h"
#include "Dial.h"
#include "Label.h"

std::string single_tmpl(
	"{ \"name\":\"single\", \"flags\":1, \"features\":[ \
		{ \"type\":\"BlobPos\", \"filters\":2047, \"constraints\":[], \"result\":[] }, \
		{ \"type\":\"BlobCount\", \"filters\":2047, \"constraints\":[0,1], \"result\":[] } \
	] }" );

std::string mb_rot_tmpl(
	"{ \"name\":\"rotate\",  \"flags\":1, \"features\":[ \
		{ \"type\":\"MultiBlobRotation\", \"filters\":31, \"constraints\":[], \"result\":[]} \
	] }" );

std::string ra_rot_tmpl(
	"{ \"name\":\"rotate\",  \"flags\":1, \"features\":[ \
		{ \"type\":\"RelativeAxisRotation\", \"filters\":31, \"constraints\":[], \"result\":[]} \
	] }" );

class MyCheckbox;

class MyDial: public Dial {
	public:

		MyDial( int _r, int _x = 0, int _y = 0, double _angle = 0.0, RGBATexture* _tex = 0 ):
			Dial(_r,_x,_y,_angle,_tex) { }

		void action( Gesture* g ) {
			Dial::action( g );
			std::cout << (char)27 << "[00;32m" << *g << std::endl;
		}
};

MyDial* dial;

MyCheckbox* single;
MyCheckbox* mb_rot;
MyCheckbox* ra_rot;

void push_gesture( std::string& tmpl ) {
	Gesture tmp;
	std::istringstream stream( tmpl );
	stream >> tmp;
	dial->region.gestures.push_back( tmp ); 
	std::cerr << (char)27 << "[01;31m" << tmp << std::endl;
}

class MyCheckbox: public Checkbox {
	public:

		MyCheckbox( int _w, int _h, int _x = 0, int _y = 0, double _angle = 0.0, RGBATexture* _tex = 0 ):
			Checkbox( _w,_h,_x,_y,_angle,_tex) { }

		void tap( Vector pos, int id ) { 

			Checkbox::tap( pos, id );

			std::cerr << (char)27 << "[2J" << (char)27 << "[;f" <<std::flush;
			dial->region.gestures.clear();

			if (single->get()) push_gesture(single_tmpl);
			if (mb_rot->get()) push_gesture(mb_rot_tmpl);
			if (ra_rot->get()) push_gesture(ra_rot_tmpl);

			dial->update();
		}
};

int main( int argc, char* argv[] ) {

	std::cout << "gispl - libTISCH 3.0 GISpL demo" << std::endl;
	std::cout << "(c) 2012 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	// create a new window: width, height, title, use_mouse
	Window* win = new Window( 800, 480, "GISpL Demo", (argc > 1) );
	win->texture( 0 );

	dial = new MyDial( 400, 160,0, 3.1 );
	dial->region.gestures.clear();
	win->add( dial );

	win->add( single = new MyCheckbox( 20,20, -350,200 ) );
	win->add( new Label( " Single Point", 200,20, -250,200 ) );

	win->add( mb_rot = new MyCheckbox( 20,20, -350,150 ) );
	win->add( new Label( " Multi-Point", 200,20, -250,150 ) );

	win->add( ra_rot = new MyCheckbox( 20,20, -350,100 ) );
	win->add( new Label( " Object Rotation", 200,20, -250,100 ) );

	win->update();
	win->run();
}

