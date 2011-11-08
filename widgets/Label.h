/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _LABEL_H_
#define _LABEL_H_

#include "Widget.h"


class TISCH_SHARED Label: public Widget {

	public:

		Label( const char* text, int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, int center = 0, int snip = 0, RGBATexture* _tex = 0 );
		virtual ~Label(); 

		virtual void draw();

		//virtual void outline( ); TODO: why does an empty outline fail?
		virtual void action( Gesture* gesture );

		void set( std::string _text );

	protected:

		std::string text;
		int center, snip;
};

#endif // _LABEL_H_

