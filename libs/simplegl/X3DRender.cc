/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "X3DRender.h"
#include "X3DTools.h"

// remap triangle-based texture coordinates to vertex-based tex coords
std::vector< TexVec >* X3DRender::getTexCoords( const TiXmlElement* element ) {

	if (!element) return 0;

	std::vector< TexVec >* tex = getList< TexVec >( element, texcoord );
	std::vector< Vector >* vec = getList< Vector >( element, vertices );
	std::vector< Vector >* nrm = getList< Vector >( element, normals  );

	if ( tex && vec && (tex->size() == vec->size()) ) return tex;

	std::vector< Triangle >* idx = getList< Triangle >( element,  indices );
	std::vector< Triangle >* txi = getList< Triangle >( element, texindex );

	if ( !tex || !vec || !nrm || !idx || !txi ) return 0;

	std::vector<TexVec> tmp(*tex);
	texcoord[element].clear();
	tex = &(texcoord[element]);

	TexVec zero; zero.set( 0.0, 0.0 );
	tex->resize( vec->size(), zero );

	int pos = 0;
	int end = idx->size();

	for ( ; pos != end; pos++ ) {
		Triangle vindex = (*idx)[pos];
		Triangle tindex = (*txi)[pos];

		if (((*tex)[vindex.x] != zero) && ((*tex)[vindex.x] != tmp[tindex.x])) {
			// eeep. now we have a problem.. there are two or more different texture
			// coordinates for vindex.a. we need to:
			// - duplicate the vertex and append it to vec, getting a new index
			// - append the new texture coords to tex, hopefully getting the same index
			// - change vindex.a to the new index
			//
			vec->push_back( (*vec)[vindex.x] );
			tex->push_back(    tmp[tindex.x] );
			nrm->push_back( (*nrm)[vindex.x] );
			unsigned int newidx = vec->size();
			(*idx)[pos].x = newidx-1;
		} else
			(*tex)[vindex.x] = tmp[tindex.x];

		if (((*tex)[vindex.y] != zero) && ((*tex)[vindex.y] != tmp[tindex.y])) {
			vec->push_back( (*vec)[vindex.y] );
			tex->push_back(    tmp[tindex.y] );
			nrm->push_back( (*nrm)[vindex.y] );
			unsigned int newidx = vec->size();
			(*idx)[pos].y = newidx-1;
		} else
			(*tex)[vindex.y] = tmp[tindex.y];

		if (((*tex)[vindex.z] != zero) && ((*tex)[vindex.z] != tmp[tindex.z])) { 
			vec->push_back( (*vec)[vindex.z] );
			tex->push_back(    tmp[tindex.z] );
			nrm->push_back( (*nrm)[vindex.z] );
			unsigned int newidx = vec->size();
			(*idx)[pos].z = newidx-1;
		} else
			(*tex)[vindex.z] = tmp[tindex.z];
	}

	return tex;
}


// generate approximate normals from surface triangles
std::vector< Vector >* X3DRender::getNormals( const TiXmlElement* element ) {

	if (!element) return 0;

	std::vector< Vector >* nrm = getList< Vector >( element, normals );
	if (nrm) return nrm;

	std::vector< Triangle >* idx = getList< Triangle >( element,  indices );
	std::vector< Vector   >* vec = getList< Vector   >( element, vertices );

	if (!idx || !vec) return 0;

	normals[element].clear();
	nrm = &(normals[element]);

	nrm->resize( vec->size() );

	std::vector<Triangle>::iterator pos = idx->begin();
	std::vector<Triangle>::iterator end = idx->end();

	for ( ; pos != end; pos++ ) {

		Vector v1,v2,v3,n;

		v1 = (*vec)[pos->x];
		v2 = (*vec)[pos->y];
		v3 = (*vec)[pos->z];

		n = (v2-v1)&(v3-v1);
		n.normalize();

		(*nrm)[pos->x] = n;
		(*nrm)[pos->y] = n;
		(*nrm)[pos->z] = n;
	}

	return nrm;
}


bool X3DRender::VisitEnter( const TiXmlDocument& doc ) { pass++; return true; }


bool X3DRender::VisitEnter( const TiXmlElement &element, const TiXmlAttribute* attrib ) {

	std::string name = element.Value();
	const TiXmlElement* parent = (const TiXmlElement*)(element.Parent());

	//
	// DEF/USE processing
	//

	for (const TiXmlAttribute* grpattr = attrib; grpattr; grpattr = grpattr->Next() ) {

		std::string action = grpattr->Name();
		std::string object = grpattr->Value();

		if ( action == "DEF" ) {
			std::map< std::string, const TiXmlElement* >::iterator it = objects.find( object );
			if ( it == objects.end() ) objects[ object ] = &element;
		}

		if ( action == "USE" ) {
			std::map< std::string, const TiXmlElement* >::iterator it = objects.find( object );
			if ( it == objects.end() ) break;
			it->second->Accept( this );
			return true;
		}
	}

	//
	// miscellaneous nodes
	//

	if (name == "Shape") {

		// generate a display list after all references have been resolved
		if (pass >= 2) {
			std::map< const TiXmlElement*, GLuint >::iterator it = lists.find( &element );
			if ( it == lists.end() ) {
				GLuint tmp = glGenLists( 1 );
				glNewList( tmp, GL_COMPILE );
				lists[ &element ] = tmp;
				finish[&element].push_back( bind( glEndList ) );
			} else {
				glCallList( it->second );
				return false;
			}
		}

		glDisable( GL_TEXTURE_2D );
		glPushMatrix();
		finish[&element].push_back( bind( glPopMatrix ) );
		return true;
	}

	if (name == "Background") {
		double r,g,b;
		for ( ; attrib; attrib = attrib->Next() ) {
			parseAttribute( attrib, "skyColor", &r, &g, &b );
		}
		glClearColor( r, g, b, 1.0 );
	}

	//
	// Text nodes
	//
	
	if (name == "Text") {
		std::string text;
		for ( ; attrib; attrib = attrib->Next() )
			parseAttribute( attrib, "string", &text );
		finish[parent].push_back( bind( glutPrint, text ) );
		return true;
	}

	//
	// Transformations
	//

	if (name == "Transform") {

		double rx,ry,rz,ra; rx = ry = rz = ra = 0.0;
		double tx,ty,tz;    tx = ty = tz =      0.0;
		double sx,sy,sz;    sx = sy = sz =      1.0;

		for ( ; attrib; attrib = attrib->Next() ) {
			parseAttribute( attrib, "translation", &tx, &ty, &tz      );
			parseAttribute( attrib, "rotation",    &rx, &ry, &rz, &ra );
			parseAttribute( attrib, "scale",       &sx, &sy, &sz      );
		}

		glPushMatrix();

		/*glTranslated(             tx, -tz, ty );
		glRotated( (180*ra/M_PI), rx, -rz, ry );
		glScaled(                 sx, -sz, sy );*/

		glTranslated(             tx, ty, tz );
		glRotated( (180*ra/M_PI), rx, ry, rz );
		glScaled(                 sx, sy, sz );

		finish[&element].push_back( bind( glPopMatrix ) );
		return true;
	}

	//
	// Appearance: Material
	//

	if (name == "Material") {
		double r,g,b,a; r = g = b = 1.0; a = 0.0;
		for ( ; attrib; attrib = attrib->Next() ) {
			parseAttribute( attrib, "diffuseColor", &r, &g, &b );
			parseAttribute( attrib, "transparency", &a         );
		}
		glColor4d( r, g, b, 1.0 - a );
		return true;
	}

	if (name == "ImageTexture") {

		bool repeatS = false;
		bool repeatT = false;
		std::string url;
		GLuint texture;

		for ( ; attrib; attrib = attrib->Next() ) {
			parseAttribute( attrib, "repeatS", &repeatS );
			parseAttribute( attrib, "repeatT", &repeatT );
			parseAttribute( attrib, "url", &url );
		}

		if ( url.empty() ) return true;

		glEnable( GL_TEXTURE_2D );

		std::map< std::string, GLuint >::iterator it = textures.find( url );
		if ( it != textures.end() ) {

			texture = it->second;
			glBindTexture( GL_TEXTURE_2D, texture );

		} else {

			glGenTextures( 1, &texture );
			textures[ url ] = texture;
			glBindTexture( GL_TEXTURE_2D, texture );

			loadTexture( url.c_str(), repeatS, repeatT );
		}

		return true;
	}

	// 
	// Geometry: Primitives
	//

	if (name == "Sphere") {
		double radius = 1.0;
		for ( ; attrib; attrib = attrib->Next() ) {
			parseAttribute( attrib, "radius", &radius );
		}
		finish[parent].push_back( bind( glutTexturedSphere, radius, 10, 10 ) );
		return true;
	}

	if (name == "Box") {
		double x,y,z; x = y = z = 2.0;
		for ( ; attrib; attrib = attrib->Next() ) {
			parseAttribute( attrib, "size", &x, &y, &z );
		}
		//finish[parent].push_back( bind( glutTexturedBox, x, -z, y ) );
		finish[parent].push_back( bind( glutTexturedBox, x, y, z ) );
		return true;
	}

	if (name == "Cylinder") {
		double radius = 1.0;
		double height = 2.0;
		for ( ; attrib; attrib = attrib->Next() ) {
			parseAttribute( attrib, "radius", &radius );
			parseAttribute( attrib, "height", &height );
		}
		glRotated( -90, 1, 0, 0 );
		glTranslated( 0, 0, -height/2 );
		finish[parent].push_back( bind( glutTexturedCylinder, radius, height, 15, 1 ) );
		return true;
	}

	if (name == "Cone") {
		double radius = 1.0;
		double height = 2.0;
		for ( ; attrib; attrib = attrib->Next() ) {
			parseAttribute( attrib, "bottomRadius", &radius );
			parseAttribute( attrib, "height",       &height );
		}
		glRotated( -90, 1, 0, 0 );
		glTranslated( 0, 0, -height/2 );
		finish[parent].push_back( bind( glutTexturedCone, radius, height, 15, 1 ) );
		return true;
	}

	//
	// Geometry: Triangle Sets
	//

	if (name == "IndexedFaceSet") {

		for ( ; attrib; attrib = attrib->Next() ) {
			parseList< Triangle >( &element, attrib,    "coordIndex",  indices );
			parseList< Triangle >( &element, attrib, "texCoordIndex", texindex );
		}

		std::vector< Triangle >* idx = getList< Triangle >( &element,  indices );
		std::vector< Vector   >* vec = getList< Vector   >( &element, vertices );
		std::vector< Vector   >* nrm = getNormals  ( &element );
		std::vector< TexVec   >* tex = getTexCoords( &element );

		if (vec) {
			glEnableClientState( GL_VERTEX_ARRAY );
			glVertexPointer( 3, GL_DOUBLE, sizeof(Vector), &((*vec)[0]) );
		} else glDisableClientState( GL_VERTEX_ARRAY );

		if (nrm) {
			glEnableClientState( GL_NORMAL_ARRAY );
			glNormalPointer( GL_DOUBLE, sizeof(Vector), &((*nrm)[0]) );
		} else glDisableClientState( GL_NORMAL_ARRAY );

		if (tex) {
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glTexCoordPointer( 2, GL_FLOAT, sizeof(TexVec), &((*tex)[0]) );
		} else glDisableClientState( GL_TEXTURE_COORD_ARRAY );

		if (idx) finish[parent].push_back( bind( glDrawElements, GL_TRIANGLES, 3*(idx->size()), GL_UNSIGNED_INT, &((*idx)[0]) ) );
		return true;
	}

	if (name == "Coordinate") {
		for ( ; attrib; attrib = attrib->Next() )
			parseList< Vector >( parent, attrib, "point", vertices );
		return true;
	}

	if (name == "TextureCoordinate") {
		for ( ; attrib; attrib = attrib->Next() )
			parseList< TexVec >( parent, attrib, "point", texcoord );
		return true;
	}

	return true;
}



bool X3DRender::VisitExit( const TiXmlElement &element ) {

	std::map< const TiXmlElement*, std::deque< Functor<void> > >::iterator tmp = finish.find( &element );

	if ( tmp != finish.end() ) {

		std::deque< Functor<void> >* funcs = &(tmp->second);

		// work through the cleanup stack
		while ( !(funcs->empty()) ) {
			(funcs->back())();
			funcs->pop_back();
		}

		finish.erase( tmp );
	}

	return true;
}

