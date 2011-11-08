/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "GLUTWrapper.h"
#include <stdio.h>

#ifdef __linux
	#define GLX_GLXEXT_LEGACY 1
	#include <GL/glx.h>
	#include <GL/glxext.h>
#endif


// function pointers for dynamically loaded OpenGL functions
pglv2ifunc   pglWindowPos2i         = (pglv2ifunc)   0xDEADBEEF;
pglv2ifunc   pglBindBuffer          = (pglv2ifunc)   0xDEADBEEF;
pglv1efunc   pglActiveTexture       = (pglv1efunc)   0xDEADBEEF;
pglv1efunc   pglClientActiveTexture = (pglv1efunc)   0xDEADBEEF;
pglv1e2ffunc pglMultiTexCoord2f     = (pglv1e2ffunc) 0xDEADBEEF;
pglv1e2dfunc pglMultiTexCoord2d     = (pglv1e2dfunc) 0xDEADBEEF;

// stub functions for returning the pointer
pglv2ifunc   getglWindowPos2i()         { return pglWindowPos2i;     }
pglv2ifunc   getglBindBuffer()          { return pglBindBuffer;      }
pglv1efunc   getglActiveTexture()       { return pglActiveTexture;   }
pglv1efunc   getglClientActiveTexture() { return pglActiveTexture;   }
pglv1e2ffunc getglMultiTexCoord2f()     { return pglMultiTexCoord2f; }
pglv1e2dfunc getglMultiTexCoord2d()     { return pglMultiTexCoord2d; }


// internal empty/fallback implementations for missing functions
void glActiveTextureINT( GLenum unit ) { }

void glBindBufferINT( GLint, GLint ) { }

void glClientActiveTextureINT( GLenum unit ) { glActiveTexture( unit ); }

void glMultiTexCoord2fINT( GLenum unit, GLfloat  x, GLfloat  y ) { glTexCoord2f( x, y ); }
void glMultiTexCoord2dINT( GLenum unit, GLdouble x, GLdouble y ) { glTexCoord2d( x, y ); }

void glWindowPos2iINT( GLint x, GLint y ) {
	glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );
	glMatrixMode( GL_MODELVIEW  ); glPushMatrix(); glLoadIdentity();
	glMatrixMode( GL_PROJECTION ); glPushMatrix(); glLoadIdentity();
	glViewport( x-1, y-1, 2, 2 );
	glRasterPos2i( 0, 0 );
	glPopMatrix(); glMatrixMode( GL_MODELVIEW ); glPopMatrix();
	glPopAttrib();
}


// GLUTWrapper class
bool g_initglut = true;

class GLUTWrapper {
	public:
		GLUTWrapper();
		void reload();
};


GLUTWrapper::GLUTWrapper( ) {

	int myargc = 1;
	char* myargv[] = { "libsimplecv", 0 };

	if (g_initglut) { glutInit( &myargc, myargv ); g_initglut = false; }
	reload();
}

void GLUTWrapper::reload() {
	getProcAddress( (const GLubyte*)"glWindowPos2i",         pglWindowPos2i,         pglv2ifunc,   glWindowPos2iINT         );
	getProcAddress( (const GLubyte*)"glBindBuffer",          pglBindBuffer,          pglv2ifunc,   glBindBufferINT          );
	getProcAddress( (const GLubyte*)"glActiveTexture",       pglActiveTexture,       pglv1efunc,   glActiveTextureINT       );
	getProcAddress( (const GLubyte*)"glClientActiveTexture", pglClientActiveTexture, pglv1efunc,   glClientActiveTextureINT );
	getProcAddress( (const GLubyte*)"glMultiTexCoord2f",     pglMultiTexCoord2f,     pglv1e2ffunc, glMultiTexCoord2fINT     );
	getProcAddress( (const GLubyte*)"glMultiTexCoord2d",     pglMultiTexCoord2d,     pglv1e2dfunc, glMultiTexCoord2dINT     );
}


GLUTWrapper loader;

