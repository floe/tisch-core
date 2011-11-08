/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "OSMRender.h"


bool OSMRender::VisitEnter( const TiXmlDocument& doc ) { pass++; return true; }

bool OSMRender::VisitEnter( const TiXmlElement &element, const TiXmlAttribute* attrib ) {

	std::string name = element.Value();
	std::string k,v;

	int id = 0,ref = 0;
	double lat = 0,lon = 0,ele = 0;

	// have we already parsed this element?
	std::map< const TiXmlElement*, Style >::iterator it = styles.find( &element );
	if (it != styles.end()) {

		Style& style = it->second;
		if (style.type == 0) return false;

		// set style
		glColor4ubv( style.color );
		glLineWidth( style.width );

		// render
		std::vector<Vector>& vert = vertices[&element];
		int count = vert.size();
		if (count == 0) return false;

		// nice stencil buffer trick for non-convex polygons
		if (style.type == GL_POLYGON) {

			glEnable( GL_STENCIL_TEST );
			glColorMask( false, false, false, false );
			
			glStencilFunc( GL_ALWAYS, 1, 0x0001 );
			glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );

			glutPaintArrays( count, &(vert[0]), (Vector*)0, (Vector*)0, style.type );

			glStencilFunc( GL_NOTEQUAL, 0, 0x0001 );
			glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

			glColorMask( true, true, true, true );
		}

		glutPaintArrays( count, &(vert[0]), (Vector*)0, (Vector*)0, style.type );
		glDisable( GL_STENCIL_TEST );

		// names
		glPushMatrix();
		glTranslated(vert[0].x,vert[0].y,vert[0].z);
		glScaled(0.00001,0.00001,0.00001);
		glutDrawString( style.name, 100, 10, 1 );
		glPopMatrix();

		// don't parse child nodes again
		return false;
	}

	// ignore relations for now
	if (name == "relation") return false;

	// get parent node for modifying associated style
	const TiXmlElement* parent = (const TiXmlElement*)(element.Parent());

	// parse attributes
	for (const TiXmlAttribute* grpattr = attrib; grpattr; grpattr = grpattr->Next() ) {

		std::string key   = grpattr->Name();
		std::string value = grpattr->Value();

		if (key ==  "id")  id = atoi(value.c_str());
		if (key == "ref") ref = atoi(value.c_str());

		if (key == "k") k = value;
		if (key == "v") v = value;

		if (key == "lat") lat = atof(value.c_str());
		if (key == "lon") lon = atof(value.c_str());
		if (key == "ele") ele = atof(value.c_str());

		if (key == "minlat") minlat = atof(value.c_str());
		if (key == "minlon") minlon = atof(value.c_str());
		if (key == "maxlat") maxlat = atof(value.c_str());
		if (key == "maxlon") maxlon = atof(value.c_str());
	}

	// map location: waypoint/POI/vertex/...
	if (name == "node") nodes[id] = Vector( lon, lat, ele );

	// node reference - append to list
	if (name == "nd") vertices[parent].push_back( nodes[ref] );

	// tags: modify parent style
	if (name == "tag") {

		if (k == "highway") {
			Style tmp = { GL_LINE_STRIP, 1, {128,128,128,255}, "", Vector() };
			if (v == "residential") tmp.width = 4;
			if (v == "primary") tmp.width = 8;
			styles[parent] = tmp;
		}

		if (k == "landuse") { // residential, forest, farm, grass, ...
			Style tmp = { GL_POLYGON, 1, {0,255,0,128}, "", Vector() };
			if (v == "residential") { tmp.color[0] = 255; tmp.color[1] =   0; tmp.color[2] =   0; tmp.color[3] = 128; }
			if (v ==  "industrial") { tmp.color[0] = 255; tmp.color[1] =   0; tmp.color[2] =   0; tmp.color[3] = 128; }
			if (v ==   "reservoir") { tmp.color[0] =   0; tmp.color[1] =   0; tmp.color[2] = 255; tmp.color[3] = 128; }
			if (v ==      "runway") { tmp.color[0] = 255; tmp.color[1] =   0; tmp.color[2] =   0; tmp.color[3] = 128; }
			if (v ==      "retail") { tmp.color[0] = 255; tmp.color[1] =   0; tmp.color[2] =   0; tmp.color[3] = 128; }
			styles[parent] = tmp;
		}

		if (k == "building") {
			Style tmp = { GL_POLYGON, 1, {255,0,0,128}, "", Vector() };
			styles[parent] = tmp;
		}

		if (k == "railway") {
			Style tmp = { GL_LINE_STRIP, 1, {0,0,0,255}, "", Vector() };
			if (v == "rail") tmp.width = 2;
			styles[parent] = tmp;
		}

		if (k == "natural") {
			Style tmp = { GL_POLYGON, 1, {0,0,255,128}, "", Vector() };
			styles[parent] = tmp;
		}

		if (k == "waterway") {
			Style tmp = { GL_LINE_STRIP, 2, {0,0,255,128}, "", Vector() };
			styles[parent] = tmp;
		}

		if (k == "name") {
			styles[parent].name = v;
		}
	}

	return true;
}



bool OSMRender::VisitExit( const TiXmlElement &element ) {

	/*std::map< const TiXmlElement*, std::deque< Functor<void> > >::iterator tmp = finish.find( &element );

	if ( tmp != finish.end() ) {

		std::deque< Functor<void> >* funcs = &(tmp->second);

		// work through the cleanup stack
		while ( !(funcs->empty()) ) {
			(funcs->back())();
			funcs->pop_back();
		}

		finish.erase( tmp );
	}*/

	return true;
}

