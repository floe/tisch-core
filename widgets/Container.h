/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "Tile.h"

#include <deque>
#include <typeinfo>

class TISCH_SHARED Container: public Tile {

	public:

		 Container( int w, int h, int x, int y, double angle = 0.0, RGBATexture* tex = 0, int mode = 0 );
		~Container();

		void add( Widget* widget );
		void raise( Widget* widget = 0 );
		void remove( Widget* widget );

		virtual void draw();
		virtual void doUpdate( Widget* target = 0, std::ostream* ost = 0 );	

		void tap( Vector vec, int id );

		template<typename T>
		void deleteWidgets()
		{
			for( int i = widgets.size()-1; i > -1; i-- )
				if(typeid(T) == typeid(*widgets[i]))
					this->remove(widgets[i]);
		} 

	private:

		std::deque<Widget*> widgets;
		int locked;

};

#endif // _CONTAINER_H_

