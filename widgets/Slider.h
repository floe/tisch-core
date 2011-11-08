/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _SLIDER_H_
#define _SLIDER_H_

#include "Button.h"

class TISCH_SHARED Slider: public Widget {

	public:

		Slider( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0 ); 
		virtual ~Slider(); 

		virtual void action( Gesture* gesture );
		virtual void draw();
		double getpos();

	protected:

		double pos;
};

#endif // _CHECKBOX_H_

