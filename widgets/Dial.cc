/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Dial.h"

#include <sstream>
#include <Motion.h>
#include <BlobCount.h>
#include <BlobPos.h>
#include <Rotation.h>

Dial::Dial( int _r, int _x, int _y, double _angle, RGBATexture* _tex ):
	Widget(_r,_r,_x,_y,_angle,_tex),
	k_angle(0), k_lower(0), k_upper(2*M_PI), oldpos(0,0,-1)
{
	getDefaultTex( _tex, "Dial.png" );

	Gesture rotate( "rotate" ); region.gestures.push_back( rotate );

	std::vector<int> limits; limits.push_back( 0 ); limits.push_back( 1 );
	BlobCount* bcnt = new BlobCount(1<<INPUT_TYPE_FINGER);
	bcnt->bounds( limits );

	// TODO: put this into a new feature? would require centroid of region..
	Gesture single( "single", GESTURE_FLAGS_STICKY );
	single.push_back( new BlobPos(1<<INPUT_TYPE_FINGER) );
	single.push_back( new Motion(1<<INPUT_TYPE_FINGER) );
	single.push_back( bcnt );
	region.gestures.push_back( single );

	// TODO: add release gesture
	region.flags( 1<<INPUT_TYPE_FINGER );
}

Dial::~Dial() { }

void Dial::outline() {
	glOutline2d( -w/4.0, -h/2.0 );
	glOutline2d(  w/4.0, -h/2.0 );
	glOutline2d(  w/2.0, -h/4.0 );
	glOutline2d(  w/2.0,  h/4.0 );
	glOutline2d(  w/4.0,  h/2.0 );
	glOutline2d( -w/4.0,  h/2.0 );
	glOutline2d( -w/2.0,  h/4.0 );
	glOutline2d( -w/2.0, -h/4.0 );
}


void Dial::draw() {

	/*texcoord[0] = GLvector( tw*1/4, 0.0    ); vertices[0] = GLvector( -w/4.0, -h/2.0 );
	texcoord[1] = GLvector( tw*3/4, 0.0    ); vertices[1] = GLvector(  w/4.0, -h/2.0 );
	texcoord[2] = GLvector( tw,     th*1/4 ); vertices[2] = GLvector(  w/2.0, -h/4.0 );
	texcoord[3] = GLvector( tw,     th*3/4 ); vertices[3] = GLvector(  w/2.0,  h/4.0 );
	texcoord[4] = GLvector( tw*3/4, th     ); vertices[4] = GLvector(  w/4.0,  h/2.0 );
	texcoord[5] = GLvector( tw*1/4, th     ); vertices[5] = GLvector( -w/4.0,  h/2.0 );
	texcoord[6] = GLvector( 0.0,    th*3/4 ); vertices[6] = GLvector( -w/2.0,  h/4.0 );
	texcoord[7] = GLvector( 0.0,    th*1/4 ); vertices[7] = GLvector( -w/2.0, -h/4.0 );*/

	enter();

	glRotated( (k_angle*180/M_PI)+180, 0, 0, 1 );
	paint();

	int val = (int)(100*get());
	std::ostringstream tmp; tmp << val;
	glutDrawString( tmp.str(), w, h/2.5, 1 );

	leave();
}


double Dial::get() { return k_angle/(k_upper-k_lower); }

void Dial::set( double _angle, double _lower, double _upper ) {
	k_angle = _angle; 
	k_lower = _lower; 
	k_upper = _upper;
}


void Dial::action( Gesture* gesture ) {

	double res = 0;

	if (gesture->name() == "single" ) {

		FeatureBase* f = (*gesture)[0];
		BlobPos* p = dynamic_cast<BlobPos*>(f);

		if (!p) return; Vector tmp = p->result();
		transform(tmp,1); tmp.z = 0; tmp.normalize();

		if (oldpos.z < 0) oldpos = tmp;
		Vector rot = oldpos & tmp;

		res = rot.z;
		oldpos = tmp;
	}

	if (gesture->name() == "release") {
		oldpos.z = -1;
	}

	if (gesture->name() == "rotate" ) {
		FeatureBase* f = (*gesture)[0];
		Rotation* r = dynamic_cast<Rotation*>(f);
		if (!r) return; res = r->result();
	}

	if (res > M_PI) res = res - 2*M_PI;
	k_angle += res;

	if (k_angle < k_lower) k_angle = k_lower;
	if (k_angle > k_upper) k_angle = k_upper;
}

