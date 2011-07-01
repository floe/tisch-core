/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Button.h"

#include <BlobCount.h>
#include <BlobPos.h>
#include <BlobID.h>

Button::Button( int _w, int _h, int _x, int _y, double _angle, RGBATexture* _tex ):
	Widget(_w,_h,_x,_y,_angle,_tex),
	active(0)
{
	getDefaultTex( _tex, "Box.png" );
	shadow = true;

	Gesture tap    ( "tap"     ); region.gestures.push_back( tap     );
	Gesture remove ( "remove"  ); region.gestures.push_back( remove  );
	Gesture release( "release" ); region.gestures.push_back( release );

	region.flags( 1<<INPUT_TYPE_FINGER );

	mycolor[0] = 0.5;
	mycolor[1] = 0.5;
	mycolor[2] = 1.0;
}

Button::~Button() { }


void Button::draw() {

	if (active) glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	Widget::draw();

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}


#ifdef TISCH_LATENCY

	#include <sys/time.h>
	extern unsigned long long us1;

	void print_latency() {
		struct timeval tv; gettimeofday(&tv,0);
		unsigned long long us2 = tv.tv_usec + tv.tv_sec * 1000000;
		std::cout << "latency: " << us2-us1 << " us" << std::endl;
	}

#endif

void Button::action( Gesture* gesture ) {

	if (gesture->name() == "tap" ) {

		#ifdef TISCH_LATENCY
			print_latency();
		#endif

		FeatureBase* f = (*gesture)[0];
		BlobID* m = dynamic_cast<BlobID*>(f);

		f = (*gesture)[1];
		BlobPos* p = dynamic_cast<BlobPos*>(f);

		Vector pos = p->result();
		transform( pos, 1 );

		active = 1;
		tap( pos, m->result() );
	}

	if (gesture->name() == "release" ) {
		// std::cout << "release" << std::endl;
		active = 0;
		release();
	}
}

void Button::tap( Vector pos, int id ) { }
void Button::release() { }

