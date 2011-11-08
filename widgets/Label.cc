/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Label.h"

Label::Label( const char* _text, int _w, int _h, int _x, int _y, double _angle, int _center, int _snip, RGBATexture* _tex ):
	Widget(_w,_h,_x,_y,_angle,_tex),
	text(_text),
	center(_center),
	snip(_snip)
{ 
	region.flags(0);
}

Label::~Label() { }


void Label::draw() {

	enter();

	if (mytex) paint_tex( mytex );

	glColor4dv( mycolor );
	glutDrawString( text, w*0.95, h*0.95, center, snip );
	
	leave();
}


void Label::set( std::string _text ) { text = _text; }

//void Label::outline( ) { }
void Label::action( Gesture* gesture ) { }

