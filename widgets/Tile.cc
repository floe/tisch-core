/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Tile.h"
#include "Container.h"

#include <Motion.h>
#include <Scale.h>
#include <Rotation.h>

Tile::Tile( int _w, int _h, int _x, int _y, double _angle, RGBATexture* _tex, int _mode ):
	Button(_w,_h,_x,_y,_angle,_tex),
	mode(_mode), slide(0), vel(0,0,0)
{
	Gesture   move(   "move" ); region.gestures.push_back( move );
	Gesture  scale(  "scale" ); region.gestures.push_back( scale );
	Gesture rotate( "rotate" ); region.gestures.push_back( rotate );
}

Tile::~Tile() { }


void Tile::tap( Vector pos, int id ) { raise(); }

void Tile::apply( Vector delta ) {

	x += delta.x;
	y += delta.y;

	vel = delta;

	if (vel.length() < 0.1) slide = 0;

	if (parent && (mode & TISCH_TILE_BOUNCE)) {
		int maxx = parent->w/2;
		int maxy = parent->h/2;
		if (parent->mode & TISCH_TILE_BBOX) {
			if (x < -maxx) { x = -maxx; vel.x = -vel.x; }
			if (x >  maxx) { x =  maxx; vel.x = -vel.x; }
			if (y < -maxy) { y = -maxy; vel.y = -vel.y; }
			if (y >  maxy) { y =  maxy; vel.y = -vel.y; }
		} else if (parent->mode & TISCH_TILE_CIRCLE) {
			int radius = (maxx>maxy?maxy:maxx);
			double len = sqrt(x*x+y*y);
			if (len > radius) {
				vel = -vel;
				x *= radius/len;
				y *= radius/len;
			}
		}
	}
}

void Tile::paint( bool update_stencil ) {
	Button::paint( update_stencil );
	if (slide) apply( vel * 0.95 );
}

void Tile::action( Gesture* gesture ) {

	Button::action( gesture );
	active = 0;

	if (mode & TISCH_TILE_SLIDE) {
		if (gesture->name() == "release") slide = 1;
		if (gesture->name() == "tap")     slide = 0;
	}

	if ((mode & TISCH_TILE_MOVE) && (gesture->name() == "move")) {
		FeatureBase* f = (*gesture)[0];
		Motion* m = dynamic_cast<Motion*>(f);
		Vector tmp = m->result();
		transform( tmp );
		apply( tmp );
		slide = 0;
	}

	if ((mode & TISCH_TILE_SCALE) && (gesture->name() == "scale")) {
		FeatureBase* f = (*gesture)[0];
		Scale* r = dynamic_cast<Scale*>(f);
		if (!r) return;
		sx *= r->result();
		sy *= r->result();
		/* TODO: configurable scale limits
		if (sx > 2.50) { sx = sy = 2.50; }
		if (sx < 0.25) { sx = sy = 0.25; }*/
	}

	if ((mode & TISCH_TILE_ROTATE) && (gesture->name() == "rotate")) {
		FeatureBase* f = (*gesture)[0];
		Rotation* r = dynamic_cast<Rotation*>(f);
		if (!r) return;
		angle += r->result();
		while(angle >= 2*M_PI) angle -= 2*M_PI;
		while(angle < 0) angle += 2*M_PI;
	}
}

