/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _X3DRENDER_H_
#define _X3DRENDER_H_


#include <vector>
#include <deque>
#include <map>

#include "GLUTWrapper.h"

#include "tinyxml.h"

#include <Functor.h>
#include <Vector.h>


class X3DRender: public TiXmlVisitor {

	public:

		X3DRender() : finish(), indices(), vertices(), normals(), texindex(), texcoord(), lists(), textures(), objects(), pass(0) { }

		virtual bool VisitEnter ( const TiXmlElement &element, const TiXmlAttribute *firstAttribute );
		virtual bool VisitExit  ( const TiXmlElement &element );

		virtual bool VisitEnter( const TiXmlDocument& doc );

	protected:

		std::vector< TexVec >* getTexCoords( const TiXmlElement* );
		std::vector< Vector >* getNormals  ( const TiXmlElement* );

		std::map< const TiXmlElement*, std::deque< Functor<void> > > finish;

		std::map< const TiXmlElement*, std::vector< Triangle > > indices;
		std::map< const TiXmlElement*, std::vector< Vector   > > vertices;
		std::map< const TiXmlElement*, std::vector< Vector   > > normals;

		std::map< const TiXmlElement*, std::vector< Triangle > > texindex;
		std::map< const TiXmlElement*, std::vector< TexVec   > > texcoord;

		std::map< const TiXmlElement*, GLuint > lists;

		std::map< std::string, GLuint > textures;

		std::map< std::string, const TiXmlElement* > objects;

		int pass;
};

#endif // _X3DRENDER_H_

