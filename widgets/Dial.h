/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _DIAL_H_
#define _DIAL_H_

#include "Widget.h"
#include "Texture.h"

class TISCH_SHARED Dial: public Widget {

	public:

		Dial( int _r, int _x = 0, int _y = 0, double _angle = 0.0, RGBATexture* _tex = 0 );
		virtual ~Dial(); 

		virtual void draw();
		virtual void action( Gesture* gesture );

		void outline();

		double get();
		void set( double _angle, double _lower, double _upper );

	protected:

		double k_angle, k_lower, k_upper;
		Vector oldpos;
};

#endif // _BUTTON_H_

