/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#include "Button.h"

class TISCH_SHARED Checkbox: public Button {

	public:

		Checkbox( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0 );
		virtual ~Checkbox(); 

		virtual void draw();

		virtual void tap( Vector pos, int id );

		int get();
		void set( int _checked );

	protected:

		int checked;
};

#endif // _CHECKBOX_H_

