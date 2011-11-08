/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "GLUTWindow.h"
#include "Texture.h"

#include <map>


typedef std::map< int, GLUTWindow* > WindowMap;

WindowMap& g_windows() {
	static WindowMap* s_map = new WindowMap();
	return *s_map;
}


// standard callbacks
void g_idle() {
	std::map< int, GLUTWindow* >::iterator pos = g_windows().begin();
	std::map< int, GLUTWindow* >::iterator end = g_windows().end();
	while (pos != end) {
		pos->second->idle();
		pos++;
	}
}

void g_display() {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->display();
}

void g_reshape( int w, int h ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->reshape( w, h );
}

void g_special( int key, int x, int y ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->keyboard( key | 0xFF0000, x, y );
}

void g_keyboard( unsigned char key, int x, int y ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->keyboard( key, x, y );
}


// standard mouse callbacks 
void g_mouse( int button, int state, int x, int y ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->mouse( -1, button, state, x, y );
}

void g_motion( int x, int y ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->motion( -1, x, y );
}

void g_passive( int x, int y ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->passive( -1, x, y );
}

void g_entry( int state ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->entry( -1, state );
}


// multi-mouse extension callbacks
void g_extentry( int num, int state ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->entry( num, state );
}

void g_extbutton( int num, int x, int y, int button, int state ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->mouse( num, button, state, x, y );
}

void g_extmotion( int num, int x, int y ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->motion( num, x, y );
}

void g_extpassive( int num, int x, int y ) {
	GLUTWindow* win = g_windows()[ glutGetWindow() ];
	if ( win ) win->passive( num, x, y );
}


// main window class
GLUTWindow::GLUTWindow( int w, int h, const std::string& title, int mode ) {

	width  = w;
	height = h;

	// glutInitWindowPosition( 100, 100 );
	glutInitDisplayMode( mode );
	glutInitWindowSize( width, height );
	win = glutCreateWindow( title.c_str() );
	g_windows()[ win ] = this;

	glutIdleFunc( g_idle );

	glutDisplayFunc( g_display );
	glutReshapeFunc( g_reshape );

	glutSpecialFunc ( g_special  );
	glutKeyboardFunc( g_keyboard );

	#ifdef GLUT_HAS_MULTI
		glutMultiEntryFunc( g_extentry );
		glutMultiButtonFunc( g_extbutton );
		glutMultiMotionFunc( g_extmotion );
		glutMultiPassiveFunc( g_extpassive );
	#elif GLUT_HAS_MPX
		glutXExtensionEntryFunc( g_extentry );
		glutXExtensionButtonFunc( g_extbutton );
		glutXExtensionMotionFunc( g_extmotion );
		glutXExtensionPassiveFunc( g_extpassive );
	#else
		glutEntryFunc( g_entry );
		glutMouseFunc( g_mouse );
		glutMotionFunc( g_motion );
		glutPassiveMotionFunc( g_passive );
	#endif
}


GLUTWindow::~GLUTWindow() {
	g_windows().erase( win );
	glutDestroyWindow( win );
}


void GLUTWindow::show( const RGBImage& img, int x, int y ) const {
	static RGBTexture* mytex = new RGBTexture( img.getWidth(), img.getHeight() );
	mytex->load( &img );
	mytex->bind();
	mytex->render();
	mytex->release();
}

void GLUTWindow::show( const IntensityImage& img, int x, int y ) const {
	static GreyTexture* mytex = new GreyTexture( img.getWidth(), img.getHeight() );
	mytex->load( &img );
	mytex->bind();
	mytex->render();
	mytex->release();
}

void GLUTWindow::show( const ShortImage& img, int x, int y ) const {
//#ifndef TISCH_IPHONE
//	typedef Texture<GL_TEXTURE_RECTANGLE_ARB,GL_LUMINANCE8,GL_LUMINANCE,GL_UNSIGNED_SHORT> ShortGreyTex;
	static ShortGreyTexture* mytex = new ShortGreyTexture( img.getWidth(), img.getHeight() );
	mytex->load( &img ); //img.getData(), GL_LUMINANCE, GL_UNSIGNED_SHORT );
	mytex->bind();
	mytex->render();
	mytex->release();
//#endif
}


void GLUTWindow::print( const std::string& text, int x, int y ) const {

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glTranslatef( width/2+x, (height-y)-7.5, 500 ); // 500 = 0.5 in Texture::render()
	glutDrawString( text, width, 15, 0, 1, 2 );
	glPopMatrix();

#if 0
	float zoom[2];

	// save and reset pixel zoom value
	glGetFloatv(GL_ZOOM_X,&(zoom[0]));
	glGetFloatv(GL_ZOOM_Y,&(zoom[1]));
	glPixelZoom( 1.0, 1.0 );

	// adapt usual screen coordinates to opengl
	y = (height - y) - 13;
	glWindowPos2i( x, y );

	// render characters
	for ( unsigned int i = 0; i < text.length(); i++ ) {
		char c = text[i];
		if (c == '\n') { y -= 13; glWindowPos2i( x, y ); }
		else glutBitmapCharacter( GLUT_BITMAP_8_BY_13, c ); 
	}

	// restore zoom
	glPixelZoom( zoom[0], zoom[1] );
#endif
}

void GLUTWindow::drawRectangleBackground( int x, int y, int size_x, int size_y, int border) const {
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	// 0/0 is in upper left corner
	glTranslatef( 0, 0, 500 ); // 500 = 0.5 in Texture::render()
	glRecti(x-border, height-y+border, x + size_x+border, height - (y + size_y+border));
	glPopMatrix();
}

void GLUTWindow::drawPolygon( std::vector<Point*> points, bool invert, int height) const
{
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glTranslatef( 0, 0, 500 ); // 500 = 0.5 in Texture::render()
	glBegin(GL_LINE_STRIP);
		if( invert )
			for(std::vector<Point*>::iterator it = points.begin(); it != points.end(); it++)
				glVertex2f( (*it)->x, height - (*it)->y );
		else
			for(std::vector<Point*>::iterator it = points.begin(); it != points.end(); it++)
				glVertex2f( (*it)->x, (*it)->y );
	glEnd();
	glPopMatrix();
}

void GLUTWindow::title( const std::string& text ) const {
	glutSetWindowTitle( text.c_str() );
}

void GLUTWindow::clear( float red, float green, float blue, float alpha ) {
	glClearColor( red, green, blue, alpha );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void GLUTWindow::mode2D() {
	
	glShadeModel( GL_FLAT );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0.0, width, 0.0, height, -1000, 1000 );
	
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POINT_SMOOTH );
	/*glEnable( GL_POLYGON_SMOOTH );
	glEnable( GL_MULTISAMPLE ); */

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}


int GLUTWindow::getWidth () { return  width; }
int GLUTWindow::getHeight() { return height; }

void GLUTWindow::swap() { glutSwapBuffers(); }
void GLUTWindow::run()  { glutMainLoop();    }


void GLUTWindow::idle( ) { }
void GLUTWindow::display( ) { }

void GLUTWindow::keyboard( int key, int x, int y ) { }
void GLUTWindow::mouse( int num, int button, int state, int x, int y ) { }
void GLUTWindow::passive( int num, int x, int y ) { }
void GLUTWindow::motion( int num, int x, int y ) { }
void GLUTWindow::entry( int num, int state ) { }


void GLUTWindow::reshape( int w, int h ) {
	width = w; height = h;
	glViewport( 0, 0, w, h );
}

