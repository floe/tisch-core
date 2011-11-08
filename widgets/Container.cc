/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Container.h"

#include <Motion.h>
#include <algorithm>
#include <stdio.h>


Container::Container( int w, int h, int x, int y, double angle, RGBATexture* tex, int mode ):
	Tile( w, h, x, y, angle, tex, mode ),
	widgets(), locked(0)
{ }

Container::~Container() {

	locked = 1;

	if (parent) parent->remove( this );

	std::deque<Widget*>::const_iterator pos = widgets.begin();
	std::deque<Widget*>::const_iterator end = widgets.end();
			
	for ( ; pos != end; pos++ ) 
		delete *pos;
}


void Container::add( Widget* widget, int back ) {

	if (locked) return;

	// if this container can move, then its child widgets are implicitly volatile
	if (mode != 0) {
		unsigned int tmp = widget->region.flags();
		widget->region.flags( tmp | REGION_FLAGS_VOLATILE );
	}

	if (back) widgets.push_back( widget );
	else widgets.push_front( widget );
	widget->parent = this;
	widget->update();
}

// FIXME: raise/lower is a mess. get rid of the fake Widget parameter.

void Container::raise( Widget* widget ) {

	if (locked) return;

	if (widget && (widget != this)) {

		remove( widget, 0 );
		add( widget );

		g_matcher->raise( (unsigned long long)widget );

	} else {

		// widget == 0: called by user, widget == this: called by parent
		if (widget != this) 
			if (parent) parent->raise(this);

		g_matcher->raise( (unsigned long long)this );

		std::deque<Widget*>::const_iterator pos = widgets.begin();
		std::deque<Widget*>::const_iterator end = widgets.end();
		
		for ( ; pos != end ; pos++ )
			if (dynamic_cast<Container*>(*pos)) (*pos)->raise(*pos);
			else g_matcher->raise( (unsigned long long)(*pos) );
	}
}

void Container::lower( Widget* widget ) {

	if (locked) return;

	if (widget && (widget != this)) {

		remove( widget, 0 );
		add( widget, 0 );

		g_matcher->lower( (unsigned long long)widget );

	} else {

		if (parent) parent->lower(this);

		g_matcher->lower( (unsigned long long)this );

		std::deque<Widget*>::const_iterator pos = widgets.begin();
		std::deque<Widget*>::const_iterator end = widgets.end();
		
		for ( ; pos != end; pos++ )
			if (dynamic_cast<Container*>(*pos)) (*pos)->lower(); // danger: this may invalidate the iterator!
			else g_matcher->lower( (unsigned long long)(*pos) );
	}
}

void Container::remove( Widget* widget, int unreg ) {

	if (locked) return;

	std::deque<Widget*>::iterator pos = std::find( widgets.begin(), widgets.end(), widget );
	widgets.erase( pos );

	widget->parent = 0;
	if (unreg) widget->unregister();
	//std::remove( widgets.begin(), widgets.end(), widget );
}


void Container::doUpdate( Widget* target ) {

	Widget::doUpdate( target );
	
	std::deque<Widget*>::const_iterator pos = widgets.begin();
	std::deque<Widget*>::const_iterator end = widgets.end();
	
	enter();

	for ( ; pos != end; pos++)
		(*pos)->doUpdate( target );

	leave();
}


void Container::paint( bool update_stencil ) {
	
	// TODO: calling raise() inside the draw function kills the iterator
	std::deque<Widget*>::const_iterator pos = widgets.begin();
	std::deque<Widget*>::const_iterator end = widgets.end();

	double step = 0.1;
	glTranslated( 0, 0, step );

	for ( ; pos != end; pos++ ) {
		double offset = step;
		Container* cont = dynamic_cast<Container*>(*pos);
		if (cont) offset = step * cont->totalHeight();
		if (!(*pos)->hide) (*pos)->draw();
		glTranslated( 0, 0, offset );
	}
}

void Container::draw() {
	enter();
	Tile::paint();
	paint();
	leave();
}


int Container::totalHeight() {
	int res = 1;
	std::deque<Widget*>::const_iterator pos = widgets.begin();
	std::deque<Widget*>::const_iterator end = widgets.end();
	for ( ; pos != end; pos++ ) {
		Container* cont = dynamic_cast<Container*>(*pos);
		if (cont) res += cont->totalHeight();
		else res++;
	}
	return res;
}

void Container::tap( Vector pos, int id ) { if (mode != 0) raise(); }

