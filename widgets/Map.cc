/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Map.h"

#include <Motion.h>
#include <Rotation.h>
#include <Scale.h>
#include <algorithm>
#include <stdio.h>


Map::Map( int w, int h, int x, int y, double angle, RGBATexture* tex, int mode ):
	Container( w, h, x, y, angle, tex, mode )
{
	cx = cy = 0;
	csx = csy = 1;
	cangle = 0;
}

Map::~Map() { }


void Map::enter( double z ) {

	glPushMatrix();

	glTranslated( x, y, 0 );
	glScaled( sx, sy, 1 );
	glRotated( 360*angle/(2*M_PI), 0, 0, 1 );

	if (z == 0.0) {
		glTranslated( cx, cy, 0 );
		glScaled( csx, csy, 1 );
		glRotated( 360*cangle/(2*M_PI), 0, 0, 1 );
	}
}


void Map::action( Gesture* gesture ) {

	//Button::action( gesture );
	active = 0;

	if ((mode & TISCH_TILE_MOVE) && (gesture->name() == "move")) {

		FeatureBase* f = (*gesture)[0];
		Motion* m = dynamic_cast<Motion*>(f);
		Vector tmp = m->result();

		tmp.x /= (asx/csx);
		tmp.y /= (asy/csy);
		tmp.rotate( absangle-cangle );

		cx += tmp.x;
		cy += tmp.y;
	}

	if ((mode & TISCH_TILE_SCALE) && (gesture->name() == "scale")) {
		FeatureBase* f = (*gesture)[0];
		Scale* r = dynamic_cast<Scale*>(f);
		if (!r) return;
		csx *= r->result();
		csy *= r->result();
	}

	if ((mode & TISCH_TILE_ROTATE) && (gesture->name() == "rotate")) {
		FeatureBase* f = (*gesture)[0];
		Rotation* r = dynamic_cast<Rotation*>(f);
		if (!r) return;
		cangle += r->result();
	}
}


void Map::draw() {

	enter(1); // only enter map coordinate system

	// set stencil buffer to 1 where drawing succeeds
	glEnable( GL_STENCIL_TEST );
	glClear( GL_STENCIL_BUFFER_BIT );
	glStencilFunc( GL_ALWAYS, 1, 0xFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

	Tile::paint( true ); // update_stencil = true

	// draw only where stencil buffer == 1
	glEnable( GL_STENCIL_TEST );
	glStencilFunc( GL_EQUAL, 1, 0xFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

	// enter internal coordinate system
	glTranslated( cx, cy, 0 );
	glScaled( csx, csy, 1 );
	glRotated( 360*cangle/(2*M_PI), 0, 0, 1 );

	Container::paint();

	glDisable( GL_STENCIL_TEST );

	leave();
}

