/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _TILE_H_
#define _TILE_H_

#include "Button.h"

#define TISCH_TILE_MOVE    1
#define TISCH_TILE_SCALE   2
#define TISCH_TILE_ROTATE  4 

#define TISCH_TILE_SLIDE   8
#define TISCH_TILE_BOUNCE 16

#define TISCH_TILE_BBOX   32
#define TISCH_TILE_CIRCLE 64

class TISCH_SHARED Tile: public Button {

	public:

		Tile( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int _mode = 0xFF ); 
		virtual ~Tile(); 

		virtual void action( Gesture* gesture );
		virtual void tap( Vector pos, int id );

		virtual void paint( bool update_stencil = false );
		virtual void apply( Vector delta );

	protected:

		int mode,slide;
		Vector vel;
};

#endif // _TILE_H_

