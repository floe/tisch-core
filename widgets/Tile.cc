/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Tile.h"

#include <Motion.h>
#include <Scale.h>
#include <Rotation.h>

Tile::Tile( int _w, int _h, int _x, int _y, double _angle, RGBATexture* _tex, int _mode ):
	Button(_w,_h,_x,_y,_angle,_tex),
	mode(_mode)
{
	Gesture   move(   "move" ); region.gestures.push_back( move );
	Gesture  scale(  "scale" ); region.gestures.push_back( scale );
	Gesture rotate( "rotate" ); region.gestures.push_back( rotate );
}

Tile::~Tile() { }


void Tile::tap( Vector pos, int id ) { raise(); }

void Tile::action( Gesture* gesture ) {

	Button::action( gesture );
	active = 0;

	if ((mode & TISCH_TILE_MOVE) && (gesture->name() == "move")) {
		FeatureBase* f = (*gesture)[0];
		Motion* m = dynamic_cast<Motion*>(f);
		Vector tmp = m->result();
		transform( tmp );
		x += tmp.x;
		y += tmp.y;
	}

	if ((mode & TISCH_TILE_SCALE) && (gesture->name() == "scale")) {
		FeatureBase* f = (*gesture)[0];
		Scale* r = dynamic_cast<Scale*>(f);
		if (!r) return;
		sx *= r->result();
		sy *= r->result();
	}

	if ((mode & TISCH_TILE_ROTATE) && (gesture->name() == "rotate")) {
		FeatureBase* f = (*gesture)[0];
		Rotation* r = dynamic_cast<Rotation*>(f);
		if (!r) return;
		angle += r->result();
	}
}

