/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _OSMRENDER_H_
#define _OSMRENDER_H_


#include <vector>
#include <deque>
#include <map>

#include "GLUTWrapper.h"

#include "tinyxml.h"

#include <Functor.h>
#include <Vector.h>


struct Style {
	int type, width;
	unsigned char color[4];
	std::string name;
	Vector pos;
};


class OSMRender: public TiXmlVisitor {

	public:

		OSMRender(): nodes(), styles(), finish(), pass(0) { }

		virtual bool VisitEnter ( const TiXmlElement &element, const TiXmlAttribute *firstAttribute );
		virtual bool VisitExit  ( const TiXmlElement &element );

		virtual bool VisitEnter( const TiXmlDocument& doc );

		double minlat,minlon,maxlat,maxlon;

	protected:

		// node list for quick ID lookup
		std::map< int, Vector > nodes;

		// styles: if an element has a style, render it
		std::map< const TiXmlElement*, Style > styles;

		// vertex lists for <way> tags
		std::map< const TiXmlElement*, std::vector<Vector> > vertices;

		std::map< const TiXmlElement*, std::deque< Functor<void> > > finish;

		int pass;

};

#endif // _OSMRENDER_H_

