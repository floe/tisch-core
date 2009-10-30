/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Label.h"

Label::Label( const char* _text, int _w, int _h, int _x, int _y, double _angle, RGBATexture* _tex ):
	Widget(_w,_h,_x,_y,_angle,_tex),
	text(_text)
{ 
	region.flags(0);
}

Label::~Label() { }


void Label::draw() {

	enter();

	glColor4dv( mycolor );

	glTranslated( -0.9*w/2.0, 0, 0 );
	glutDrawString( text, w*0.95, h*0.95 );
	
	leave();
}


void Label::set( std::string _text ) { text = _text; }

//void Label::outline( ) { }
void Label::action( Gesture* gesture ) { }

