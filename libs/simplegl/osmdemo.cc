/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "OSMRender.h"
#include <GLUTWindow.h>

OSMRender osm;
TiXmlDocument* doc;
GLUTWindow* win;

// view parameters
int zoom = 30000;
int x = 400, y = 300;
int base_x,base_y;


void motion( int mouse_x, int mouse_y ) {
  x += mouse_x - base_x; base_x = mouse_x;
  y -= mouse_y - base_y; base_y = mouse_y;
  glutPostRedisplay();
}

void click( int button, int state, int mouse_x, int mouse_y ) {
  if (button == 3) { zoom -= 1000; glutPostRedisplay(); return; }
  if (button == 4) { zoom += 1000; glutPostRedisplay(); return; }
  if (button == GLUT_LEFT_BUTTON) {
  	if (state != GLUT_DOWN) return;
  	base_x = mouse_x;
  	base_y = mouse_y;
	}
}


void display() {

	win->mode2D();
	win->clear(1.0,1.0,1.0);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glTranslated(x,y,0);
	glScaled(zoom,zoom,zoom);
	double tx = (osm.minlon+osm.maxlon)/2;
	double ty = (osm.minlat+osm.maxlat)/2;
  glTranslated(-tx,-ty,0.0);

	glDisable( GL_LINE_SMOOTH );
	doc->Accept(&osm);

	glutSwapBuffers();
}

void kbd( unsigned char key, int, int ) {
	if (key == 'q') exit(1);
}

int main(int argc, char* argv[]) {

	if (argc < 2) return 1;

	doc = new TiXmlDocument(argv[1]);
	doc->LoadFile();

	win = new GLUTWindow(800,600,"gldemo");

	// make functions known to GLUT
	glutDisplayFunc( display );
	glutMotionFunc( motion );
	glutKeyboardFunc( kbd );
	glutMouseFunc( click );
	glutIdleFunc( 0 );

	win->run();

	return 0;
}

