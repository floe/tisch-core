/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _X3DTOOLS_H_
#define _X3DTOOLS_H_

#include <sstream>
#include <vector>
#include <map>

#include "GLUTWrapper.h"

#include "tinyxml.h"
#include <Vector.h>

/*#include <stdio.h>
#define glError() { \
        GLenum err = glGetError(); \
        while (err != GL_NO_ERROR) { \
                fprintf(stderr, "glError: %s caught at %s:%u\n", (char *)gluErrorString(err), __FILE__, __LINE__); \
                err = glGetError(); \
        } \
}*/

GLfloat unproject(int screen_x, int screen_y, Vector* click, Vector* origin, GLfloat screen_z = -1.0);


void glutTexturedSphere( GLdouble radius, GLint slices, GLint stacks );
void glutTexturedCylinder( GLdouble radius, GLdouble height, GLint slices, GLint stacks );
void glutTexturedCone( GLdouble base, GLdouble height, GLint slices, GLint stacks );
void glutTexturedBox( GLdouble x, GLdouble y, GLdouble z );

void glutPrint( std::string text ); 

void loadTexture( const char* url, bool repeatS, bool repeatT );


int parseAttribute( const TiXmlAttribute* attrib, const std::string& name, bool* res );
int parseAttribute( const TiXmlAttribute* attrib, const std::string& name, std::string* res );

int parseAttribute(
	const TiXmlAttribute* attrib,
	const std::string& name,
	double* res0,
	double* res1 = 0,
	double* res2 = 0,
	double* res3 = 0
);




void process( std::istringstream& stream, std::vector< Triangle >* storage );
void process( std::istringstream& stream, std::vector< Vector   >* storage );
void process( std::istringstream& stream, std::vector< TexVec   >* storage );


template< class Type > std::vector< Type >* getList(
	const TiXmlElement* element, 
	std::map< const TiXmlElement*, std::vector< Type > >& storage
) {
	typename std::map< const TiXmlElement*, std::vector< Type > >::iterator it = storage.find( element );
	if ( it != storage.end() ) return &(it->second);
	return 0;
}

template< class Type> void printList( std::vector< Type >* foo ) {
	if (!foo) return;
	typename std::vector<Type>::iterator pos = foo->begin();
	typename std::vector<Type>::iterator end = foo->end();
	for ( ; pos != end; pos++) std::cout << *pos << " ";
	std::cout << std::endl;
}

template< class Type > std::vector< Type >* parseList(
	const TiXmlElement* element, 
	const TiXmlAttribute* attrib, 
	const std::string& name,
	std::map< const TiXmlElement*, std::vector< Type > >& storage
) {
	if ( attrib->Name() != name ) return 0;
	std::vector< Type >* tmp = getList< Type >( element, storage );
	if (tmp) return tmp;
	std::istringstream stream( attrib->Value() );
	storage[element].clear();
	tmp = &(storage[element]);
	process( stream, tmp );
	return tmp;
}

#endif // _X3DTOOLS_H_

