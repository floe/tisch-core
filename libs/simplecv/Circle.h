/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Vector.h"

class TISCH_SHARED Circle {

	public:

		Circle( Point c, int radius );
		virtual ~Circle(); 
		void follow();

	protected:

		int x,y,d,cx,cy;

		virtual void foreach( int x, int y ) = 0;
};


class TISCH_SHARED CirclePoints: public Circle {

	public:

		CirclePoints( Point _c, int _radius );
		virtual ~CirclePoints();

		std::vector<Point> points;

	protected:

		virtual void foreach( int x, int y );
};

