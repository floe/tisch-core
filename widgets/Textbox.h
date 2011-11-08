/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _TEXTBOX_H_
#define _TEXTBOX_H_

#include "Widget.h"
#include "Texture.h"

class TISCH_SHARED Textbox: public Widget {

	public:

		Textbox( int _w, int _h0, int _h1, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0 );
		virtual ~Textbox(); 

		virtual void draw();
		virtual void action( Gesture* gesture );

		std::string get();
		void set( std::string _text );

	protected:

		double h0,h1,diff;
		int active,kbd;
		std::string text;
};

#endif // _TEXTBOX_H_

