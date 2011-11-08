/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

// This file tries to hide all the subtle differences which still exist
// between the Windows, Linux and MacOS implementations of OpenGL. Also
// provides some convenience functions, e.g. for handling vertex arrays.

#ifndef _GLUTWRAPPER_H_
#define _GLUTWRAPPER_H_


#if TISCH_IPHONE

	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>

	// OpenGL -> GLES replacement definitions
	#define GLdouble  GLfloat
	#define GL_DOUBLE GL_FLOAT

	#define glOrtho glOrthof
	#define glGetDoublev glGetFloatv
	#define glTranslated glTranslatef
	#define glRotated glRotatef
	#define glScaled glScalef
 
	#define glMultiTexCoord2f(u,x,y) glMultiTexCoord4f(u,x,y,0,1)
	#define glMultiTexCoord2d(u,x,y) glMultiTexCoord4f(u,x,y,0,1)

	#define GL_RGB8 GL_RGB
	#define GL_RGBA8 GL_RGBA
	#define GL_LUMINANCE8 GL_LUMINANCE
	#define GL_CLAMP GL_CLAMP_TO_EDGE

	// OpenGL empty definitions
	#define GL_ENABLE_BIT    1
	#define GL_TRANSFORM_BIT 2
	#define GL_VIEWPORT_BIT  4

	#define glPushAttrib(x)
	#define glPopAttrib()

	#define GL_PIXEL_PACK_BUFFER 0
	#define glGetTexImage(a,b,c,d,e)

	// GLU definitions
	GLint gluProject(
		GLdouble objx, GLdouble objy, GLdouble objz, 
		const GLdouble modelMatrix[16], 
		const GLdouble projMatrix[16],
		const GLint viewport[4],
		GLdouble *winx, GLdouble *winy, GLdouble *winz
	);

	// GLUT empty definitions
	#define GLUT_DEPTH  1
	#define GLUT_RGBA   2
	#define GLUT_DOUBLE 4 

	#define glutGetWindow() 1
	#define glutGetModifiers() 0
	#define glutSetWindowTitle(x)

	// GLUT replacement definitions
	#define GLUT_HAS_MULTI 1

	#define GLUT_ENTERED 1 
	#define GLUT_LEFT    2

	#define GLUT_DOWN 1
	#define GLUT_UP   2 

	void glutInitDisplayMode( unsigned int mode );
	void glutInitWindowSize( int width, int height );
	int  glutCreateWindow( const char *name );

	void glutIdleFunc(void (*func)(void));
	void glutDisplayFunc(void (*func)(void));
	void glutReshapeFunc(void (*func)(int width, int height));

	void glutSpecialFunc(void (*func)(int key, int x, int y));
	void glutKeyboardFunc(void (*func)(unsigned char key, int x, int y));

	void glutEntryFunc( void (*func)(int state) );
	void glutMouseFunc( void (*func)(int button, int state, int x, int y) );
	void glutMotionFunc( void (*func)(int x, int y) );
	void glutPassiveMotionFunc( void (*func)(int x, int y) );

	void glutMultiEntryFunc( void (*func)(int num, int state) );
	void glutMultiButtonFunc( void (*func)(int num, int x, int y, int button, int state) );
	void glutMultiMotionFunc( void (*func)(int num, int x, int y) );
	void glutMultiPassiveFunc( void (*func)(int num, int x, int y) );

	void glutDestroyWindow( int win );
	void glutPostRedisplay();
	void glutSwapBuffers();
	void glutFullScreen();
	void glutMainLoop();

#elif __APPLE__

	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>

	#define getProcAddress(a,b,c,d)

#elif _MSC_VER

	#include <winsock2.h> //must be included before windows.h
	#include <windows.h> //must be included before gl.h
	#include <GL/gl.h>
	#include <GL/glut.h>

	#define getProcAddress(a,b,c,d) b = (c)wglGetProcAddress(LPCSTR(a)); if (!b) b = d

#else

	#define GL_GLEXT_PROTOTYPES
	#include <GL/glut.h>
	#include <GL/glext.h>

	#define getProcAddress(a,b,c,d) b = (c)glXGetProcAddressARB(a); if (!b) b = d

#endif


// check if we have freeglut
#ifdef FREEGLUT
	#include <GL/freeglut_ext.h>
#endif


// fallbacks for defines from glext.h
#define GL_BGRA                  0x80E1
#define GL_DEPTH_COMPONENT32     0x81A7
#define GL_TEXTURE0              0x84C0
#define GL_RGBA32F_ARB           0x8814
#define GL_PIXEL_PACK_BUFFER     0x88EB


// function prototypes for dynamically loaded functions
typedef void(*pglv1efunc)(GLenum);
typedef void(*pglv2ifunc)(GLint, GLint);
typedef void(*pglv1e2ffunc)(GLenum, GLfloat, GLfloat);
typedef void(*pglv1e2dfunc)(GLenum, GLdouble, GLdouble);


#ifndef __APPLE__

	#ifndef glWindowPos2i
		TISCH_SHARED pglv2ifunc getglWindowPos2i();
		#define glWindowPos2i getglWindowPos2i()
	#endif

	#ifndef glBindBuffer
		TISCH_SHARED pglv2ifunc getglBindBuffer();
		#define glBindBuffer getglBindBuffer()
	#endif

	#ifndef glActiveTexture
		TISCH_SHARED pglv1efunc getglActiveTexture();
		#define glActiveTexture getglActiveTexture()
	#endif

	#ifndef glClientActiveTexture
		TISCH_SHARED pglv1efunc getglClientActiveTexture();
		#define glClientActiveTexture getglClientActiveTexture()
	#endif

	#ifndef glMultiTexCoord2f
		TISCH_SHARED pglv1e2ffunc getglMultiTexCoord2f();
		#define glMultiTexCoord2f getglMultiTexCoord2f()
	#endif

	#ifndef glMultiTexCoord2d
		TISCH_SHARED pglv1e2dfunc getglMultiTexCoord2d();
		#define glMultiTexCoord2d getglMultiTexCoord2d()
	#endif

#endif


// template helper function to paint a vertex array with one 2D texture and optional normals
template <class T> void glutPaintArrays(

	int num,
	T* vertices, 
	T* texcoord = 0, 
	T* normals = 0, 
	GLenum mode = GL_TRIANGLE_STRIP,
	GLenum where = GL_TEXTURE0

) {

	GLenum type = GL_DOUBLE;
	if (sizeof(T) == 3*sizeof(GLfloat)) type = GL_FLOAT;

	glClientActiveTexture( where );

	if (vertices) { glEnableClientState( GL_VERTEX_ARRAY        ); glVertexPointer  ( 3, type, sizeof(T), vertices ); }
	if (texcoord) { glEnableClientState( GL_TEXTURE_COORD_ARRAY ); glTexCoordPointer( 2, type, sizeof(T), texcoord ); }
	if (normals ) { glEnableClientState( GL_NORMAL_ARRAY        ); glNormalPointer  (    type, sizeof(T), normals  ); }

	glDrawArrays( mode, 0, num );
}


#define GLUT_FONT_HEIGHT  152.381
#define GLUT_FONT_DESCENT  33.333
#define GLUT_FONT_WIDTH   104.762

// template helper function to draw a string at the center of the local coordinate system
template <class T> void glutDrawString( const T& text, GLdouble w, GLdouble h, int center = 0, int snip = 0, double size = 3 ) {

	double factor = h/GLUT_FONT_HEIGHT; 

	const char* chr = text.c_str();
	int len = text.length();
	int space = (int)(w/(GLUT_FONT_WIDTH*factor));
	//int shift = len - (int)(w/(GLUT_FONT_WIDTH*factor)) + 1;
	//if ((shift > 0) && (shift < len)) chr += shift;

	// not enough space in bbox?
	if (space < len) { 
		if (snip) chr += (len - space); // either cut the string
		else factor = w/(GLUT_FONT_WIDTH*len); // or scale by width
	}

	glLineWidth( size );
	glPointSize( size - 1 );

	glScaled( factor, factor, 1 );

	double xshift = - (w/factor)/2.0;
	if (center) xshift = - ((double)len/2.0)*GLUT_FONT_WIDTH;
	glTranslated( xshift, -0.5*(GLUT_FONT_HEIGHT-GLUT_FONT_DESCENT), 0 );

	while (*chr) glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,*(chr++));
}


#endif // _GLUTWRAPPER_H_

