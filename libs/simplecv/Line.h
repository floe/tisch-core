/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Vector.h"

class TISCH_SHARED Line {

	public:

		Line( Point p1, Point p2 );
		virtual ~Line(); 
		void follow();

	protected:

		int x,y,d,num,dx,dy;
		int d1,d2,x1,x2,y1,y2;

		virtual void foreach( int x, int y ) = 0;
};


class TISCH_SHARED LinePoints: public Line {

	public:

		LinePoints( Point _p1, Point _p2 );
		virtual ~LinePoints();

		std::vector<Point> points;

	protected:

		virtual void foreach( int x, int y );
};

