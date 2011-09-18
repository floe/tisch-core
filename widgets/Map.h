/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MAP_H_
#define _MAP_H_

#include "Container.h"

class TISCH_SHARED Map: public Container {

	public:

		 Map( int w, int h, int x, int y, double angle = 0.0, RGBATexture* tex = 0, int mode = 0 );
		~Map();

		virtual void draw();
		virtual void enter( double z = 0.0 );
		virtual void action( Gesture* gesture );

	protected:
		double cx,cy,csx,csy,cangle;
};

#endif // _MAP_H_

