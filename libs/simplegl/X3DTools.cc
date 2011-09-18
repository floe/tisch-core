/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <fstream>

#include "X3DTools.h"


int lpot( int val ) {
	for (int i = 0; i < 16; i++) {
		int j = 1 << i;
		if (j >= val) return j;
	}
	return -1;
}

// get world coordinates from screen coordinates
GLfloat unproject(int screen_x, int screen_y, Vector* click, Vector* origin, GLfloat screen_z) {

  GLdouble projection[16];
  GLdouble modelview [16];
  GLdouble world[3];
  GLint viewport[4];

  // get current matrices.. 
  glGetDoublev(GL_PROJECTION_MATRIX,projection);
  glGetDoublev(GL_MODELVIEW_MATRIX ,modelview );

  // ..and viewport
  glGetIntegerv(GL_VIEWPORT,viewport);

  // viewport[3] is height of window in pixels
  screen_y = viewport[3] - (screen_y-1);

  // get z value from depth buffer
  if (screen_z < 0) glReadPixels(screen_x,screen_y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&screen_z);
  
  // reverse projection of clicked point
  gluUnProject(screen_x,screen_y,screen_z,modelview,projection,viewport,&world[0],&world[1],&world[2]);
  click->x = world[0]; click->y = world[1]; click->z = world[2];
  
  // reverse projection of origin
  gluUnProject(viewport[2]/2,viewport[3]/2,0.0,modelview,projection,viewport,&world[0],&world[1],&world[2]);
  origin->x = world[0]; origin->y = world[1]; origin->z = world[2];

  // return screen_z for background checking
  return screen_z;
}


void glutTexturedSphere( GLdouble radius, GLint slices, GLint stacks ) {
	GLUquadricObj* tmp = gluNewQuadric();
	gluQuadricTexture( tmp, GL_TRUE );
	gluSphere( tmp, radius, slices, stacks );
	gluDeleteQuadric( tmp );
}

void glutTexturedCone( GLdouble base, GLdouble height, GLint slices, GLint stacks ) {
	GLUquadricObj* tmp = gluNewQuadric();
	gluQuadricTexture( tmp, GL_TRUE );
	gluQuadricOrientation ( tmp, GLU_OUTSIDE );
	gluCylinder( tmp, base, 0, height, slices, stacks );
	gluQuadricOrientation ( tmp, GLU_INSIDE );
	gluDisk( tmp, 0, base, slices, stacks );
	//glTranslated( 0, 0, height );
	//gluQuadricOrientation ( tmp, GLU_OUTSIDE );
	//gluDisk( tmp, 0, radius, slices, stacks );
	gluDeleteQuadric( tmp );
}

void glutTexturedCylinder( GLdouble radius, GLdouble height, GLint slices, GLint stacks ) {
	GLUquadricObj* tmp = gluNewQuadric();
	gluQuadricTexture( tmp, GL_TRUE );
	gluQuadricOrientation ( tmp, GLU_OUTSIDE );
	gluCylinder( tmp, radius, radius, height, slices, stacks );
	gluQuadricOrientation ( tmp, GLU_INSIDE );
	gluDisk( tmp, 0, radius, slices, stacks );
	glTranslated( 0, 0, height );
	gluQuadricOrientation ( tmp, GLU_OUTSIDE );
	gluDisk( tmp, 0, radius, slices, stacks );
	gluDeleteQuadric( tmp );
}

void glutTexturedBox( GLdouble x, GLdouble y, GLdouble z ) {

	x = x/2.0;
	y = y/2.0;
	z = z/2.0;

	glBegin(GL_QUADS);

		// x-z plane
		glTexCoord2f( 0, 1 ); glNormal3f( 0,  1, 0 ); glVertex3d( -x,  y, -z );
		glTexCoord2f( 0, 0 ); glNormal3f( 0,  1, 0 ); glVertex3d( -x,  y,  z );
		glTexCoord2f( 1, 0 ); glNormal3f( 0,  1, 0 ); glVertex3d(  x,  y,  z );
		glTexCoord2f( 1, 1 ); glNormal3f( 0,  1, 0 ); glVertex3d(  x,  y, -z );

		glTexCoord2f( 1, 1 ); glNormal3f( 0, -1, 0 ); glVertex3d( -x, -y, -z );
		glTexCoord2f( 1, 0 ); glNormal3f( 0, -1, 0 ); glVertex3d( -x, -y,  z );
		glTexCoord2f( 0, 0 ); glNormal3f( 0, -1, 0 ); glVertex3d(  x, -y,  z );
		glTexCoord2f( 0, 1 ); glNormal3f( 0, -1, 0 ); glVertex3d(  x, -y, -z );

		// y-z plane
		glTexCoord2f( 1, 1 ); glNormal3f(  1, 0, 0 ); glVertex3d(  x, -y, -z );
		glTexCoord2f( 1, 0 ); glNormal3f(  1, 0, 0 ); glVertex3d(  x, -y,  z );
		glTexCoord2f( 0, 0 ); glNormal3f(  1, 0, 0 ); glVertex3d(  x,  y,  z );
		glTexCoord2f( 0, 1 ); glNormal3f(  1, 0, 0 ); glVertex3d(  x,  y, -z );

		glTexCoord2f( 0, 1 ); glNormal3f( -1, 0, 0 ); glVertex3d( -x, -y, -z );
		glTexCoord2f( 0, 0 ); glNormal3f( -1, 0, 0 ); glVertex3d( -x, -y,  z );
		glTexCoord2f( 1, 0 ); glNormal3f( -1, 0, 0 ); glVertex3d( -x,  y,  z );
		glTexCoord2f( 1, 1 ); glNormal3f( -1, 0, 0 ); glVertex3d( -x,  y, -z );

		// x-y plane
		glTexCoord2f( 0, 1 ); glNormal3f( 0, 0,  1 ); glVertex3d( -x, -y,  z );
		glTexCoord2f( 0, 0 ); glNormal3f( 0, 0,  1 ); glVertex3d(  x, -y,  z );
		glTexCoord2f( 1, 0 ); glNormal3f( 0, 0,  1 ); glVertex3d(  x,  y,  z );
		glTexCoord2f( 1, 1 ); glNormal3f( 0, 0,  1 ); glVertex3d( -x,  y,  z );

		glTexCoord2f( 1, 1 ); glNormal3f( 0, 0, -1 ); glVertex3d( -x, -y, -z );
		glTexCoord2f( 1, 0 ); glNormal3f( 0, 0, -1 ); glVertex3d(  x, -y, -z );
		glTexCoord2f( 0, 0 ); glNormal3f( 0, 0, -1 ); glVertex3d(  x,  y, -z );
		glTexCoord2f( 0, 1 ); glNormal3f( 0, 0, -1 ); glVertex3d( -x,  y, -z );

	glEnd();
}

void glutPrint( std::string text ) {
	glScaled( 0.01, 0.01, 0.01 );
	glLineWidth( 2.0 );
	//glEnable( GL_LINE_SMOOTH );
	for ( const char* tmp = text.c_str(); *tmp; tmp++ )
		glutStrokeCharacter( GLUT_STROKE_ROMAN, *tmp );
}


void loadTexture( const char* url, bool repeatS, bool repeatT ) {

	std::string magic,tmp;
	int fwidth,fheight;
	int twidth,theight;
	int fbpp,size;
	char* data;

	// open file with whitespace skipping
	std::ifstream myfile( url, std::ios::in );
	myfile >> std::skipws;

	// parse the header
	myfile >> magic;   myfile.ignore(1); if (myfile.peek() == '#') getline( myfile, tmp );
	myfile >> fwidth;  myfile.ignore(1); if (myfile.peek() == '#') getline( myfile, tmp );
	myfile >> fheight; myfile.ignore(1); if (myfile.peek() == '#') getline( myfile, tmp );
	myfile >> fbpp;

	if ((magic != "P6") || (fbpp > 255) || (fbpp < 1)) return;

	size = fwidth*fheight*3; 
	data = new char[size];

	// skip one byte, read the rest
	myfile.ignore( 1 );
	myfile.read( data, size );
	myfile.close( );

	// scale to next power of two, if necessary
	twidth  = lpot( fwidth  );
	theight = lpot( fheight );

	if ((twidth != fwidth) || (theight != fheight)) {
		char* tmp = data;
		data = new char[twidth*theight*3];
		gluScaleImage( GL_RGB, fwidth, fheight, GL_UNSIGNED_BYTE, tmp, twidth, theight, GL_UNSIGNED_BYTE, data );
		delete[] tmp;
	}

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data );

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (repeatS ? GL_REPEAT : GL_CLAMP) );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (repeatT ? GL_REPEAT : GL_CLAMP) );

	delete[] data;
}


int parseAttribute( const TiXmlAttribute* attrib, const std::string& name, bool* res ) {
	if ( attrib->Name() != name ) return 0;
	std::string tmp( attrib->Value() );
	if (tmp == "0") *res = false;
	if (tmp == "1") *res = true;
	if (tmp == "FALSE") *res = false;
	if (tmp ==  "TRUE") *res = true;
	return 1;
}

int parseAttribute( const TiXmlAttribute* attrib, const std::string& name, std::string* res ) {
	if ( attrib->Name() != name ) return 0;
	*res = attrib->ValueStr();
	return 1;
}

int parseAttribute(
	const TiXmlAttribute* attrib,
	const std::string& name,
	double* res0,
	double* res1,
	double* res2,
	double* res3
) {
	if ( attrib->Name() != name ) return 0;
	std::istringstream tmp( attrib->Value() );
	if (res0) tmp >> (*res0);
	if (res1) tmp >> (*res1);
	if (res2) tmp >> (*res2);
	if (res3) tmp >> (*res3);
	return 1;
}


void process( std::istringstream& stream, std::vector<Vector>* storage ) {
	Vector vertex;
	while ( stream ) {
		stream >> vertex.x >> vertex.y >> vertex.z;
		if (stream) storage->push_back( vertex );
		stream.ignore(1);
	}
}

void process( std::istringstream& stream, std::vector<TexVec>* storage ) {
	TexVec vector;
	while ( stream ) {
		stream >> vector.x >> vector.y;
		vector.y = 1.0 - vector.y;
		if (stream) storage->push_back( vector );
		stream.ignore(1);
	}
}

void process( std::istringstream& stream, std::vector<Triangle>* storage ) {

	Triangle current,prev;
	int index, count = 0;

	while ( stream ) {

		if (count == 0) {
			stream >> prev.x; stream.ignore(1);
			stream >> prev.y; stream.ignore(1);
			stream >> prev.z; 
			if (stream) { storage->push_back( prev ); count++; }
			stream.ignore(1);
			continue;
		}

		stream >> index;
		if (index == -1) { count = 0; stream.ignore(1); continue; }

		current.set( prev.x, prev.z, index );
		if (stream) { storage->push_back( current ); count++; }
		stream.ignore(1);
		prev = current;
	}
}

