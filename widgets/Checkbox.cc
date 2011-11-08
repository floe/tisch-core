/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Checkbox.h"


Checkbox::Checkbox( int _w, int _h, int _x, int _y, double _angle, RGBATexture* _tex ):
	Button(_w,_h,_x,_y,_angle,_tex),
	checked(0)
{ }

Checkbox::~Checkbox() { }


void Checkbox::draw() {

	enter();
	Widget::paint();

	if (checked) {

		glLineWidth(5.0);
		GLvector vertices[4];

		vertices[0] = GLvector( -w/2.5, -h/2.5, 0.0 );
		vertices[1] = GLvector(  w/2.5,  h/2.5, 0.0 );
		vertices[2] = GLvector( -w/2.5,  h/2.5, 0.0 );
		vertices[3] = GLvector(  w/2.5, -h/2.5, 0.0 );

		glutPaintArrays( 4, vertices, (GLvector*)0, (GLvector*)0, GL_LINES );
	}

	leave();
}

int Checkbox::get() { return checked; }

void Checkbox::set(int _checked) { checked = _checked; }

void Checkbox::tap( Vector pos, int id ) { checked = !checked; }

