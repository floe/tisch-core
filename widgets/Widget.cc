/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <Widget.h>
#include <Container.h>

void Widget::update( Widget* target ) {
	if (target == 0) target = this;
	//std::cout << "update " << (unsigned long long int)target << std::endl;
	if (parent == 0)
		//Problem: hier wird doUpdate von Container, anstatt von MasterContainer aufgerufen.
		doUpdate( target );
	else
		parent->update( target );
}

void Widget::raise( Widget* widget ) { if (parent) parent->raise( this ); }
void Widget::lower( Widget* widget ) { if (parent) parent->lower( this ); }

void Widget::transform( Vector& vec, int abs, int local ) { 
	if (abs) vec = vec - abspos; 
	if (parent && !local) {
		vec.x /= parent->asx;
		vec.y /= parent->asy;
		vec.rotate( parent->absangle );
	} else {
		vec.x /= asx;
		vec.y /= asy;
		vec.rotate( absangle );
	}
}

