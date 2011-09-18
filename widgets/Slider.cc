/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Slider.h"

#include <Motion.h>

Slider::Slider( int _w, int _h, int _x, int _y, double _angle, RGBATexture* _tex ):
	Widget(_w,_h,_x,_y,_angle,_tex),
	pos(0.0)
{
	getDefaultTex( _tex, "Box.png" );
	shadow = true;

	Gesture move( "move" ); region.gestures.push_back( move );
	region.flags( 1 << INPUT_TYPE_FINGER );

	mycolor[0] = 0.5;
	mycolor[1] = 0.5;
	mycolor[2] = 0.75;
}

Slider::~Slider() { }


void Slider::draw() {

	enter();

	GLvector vertices[3];

	vertices[0] = GLvector( -w/2.0,    0.0, 0.0 );
	vertices[1] = GLvector(  w/2.0, -h/4.0, 0.0 );
	vertices[2] = GLvector(  w/2.0,  h/4.0, 0.0 );

	glColor4dv( mycolor );
	glutPaintArrays( 3, vertices );

	glTranslated( (pos-0.5)*(w-h), 0, 0 );
	glScaled( h/w, 1.0, 1.0 );

	Widget::paint();

	leave();
}


void Slider::action( Gesture* gesture ) {

	if (gesture->name() == "move") {
		FeatureBase* f = (*gesture)[0];
		Motion* m = dynamic_cast<Motion*>(f);
		Vector tmp = m->result();
		transform( tmp, 0, 1 );
		pos += tmp.x/w;
		if (pos < 0.0) pos = 0.0;
		if (pos > 1.0) pos = 1.0;
	}
}

double Slider::getpos() {
	return pos;
}

