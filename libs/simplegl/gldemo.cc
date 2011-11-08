/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "X3DRender.h"
#include "X3DTools.h"
#include <GLUTWindow.h>

X3DRender alien;
TiXmlDocument* doc;

// view parameters
GLfloat spinx = -60.0;
GLfloat spiny =   0.0;
GLfloat spinz =   0.0;
GLfloat zoom  =  5;

GLfloat base_x,base_z;
int drag_x,drag_y;
GLuint list = -1;

void motion(int mouse_x, int mouse_y) {
  spinz = base_z + (drag_x - mouse_x);
  spinx = base_x + (drag_y - mouse_y);
  glutPostRedisplay();
}

void click(int button, int state, int mouse_x, int mouse_y) {
  if (button == 3) { zoom -= 0.1; glutPostRedisplay(); return; }
  if (button == 4) { zoom += 0.1; glutPostRedisplay(); return; }
  if (button == GLUT_MIDDLE_BUTTON) {
  	if (state != GLUT_DOWN) return;
  	drag_x = mouse_x; base_x = spinx;
  	drag_y = mouse_y; base_z = spinz;
		motion(mouse_x,mouse_y);
  }
  if (button == GLUT_LEFT_BUTTON) {
  	if (state != GLUT_DOWN) return;
		Vector coord;
		Vector orig;
		GLfloat tmp = unproject(mouse_x,mouse_y,&coord,&orig);
		if (tmp == 1) return;
		std::cout << coord << std::endl << std::endl;
		//std::cout << orig << std::endl;
	}
}


void display()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

  glTranslatef(0.0,0.0,-zoom);

  glRotatef(spinx,1.0,0.0,0.0);
  glRotatef(spiny,0.0,1.0,0.0);
  glRotatef(spinz,0.0,0.0,1.0);

	GLfloat light_pos[] = { 0, 0, 1, 0.0 };
	glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
	
	/*glLoadIdentity();

  glTranslatef(0.0,0.0,-zoom);

  glRotatef(-60,1.0,0.0,0.0);
  glRotatef( 0 ,0.0,1.0,0.0);
  glRotatef(-60,0.0,0.0,1.0);*/

	doc->Accept(&alien);
	//printf("%s\n",gluErrorString(glGetError()));
	//alien.printObjects();

	glutSwapBuffers();
}

void resize( int m_width, int m_height ){
	glViewport( 0, 0, m_width, m_height );
	// create a perspective projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 50, ((double)m_width/(double)m_height), 0.01, 1000 );
	glutPostRedisplay();
}

void kbd( unsigned char key, int, int ) {
	if (key == 't') glDisable(GL_TEXTURE_2D);
	if (key == 'T') glEnable(GL_TEXTURE_2D);
	if (key == 'q') exit(1);
	glutPostRedisplay();
}


int main(int argc, char* argv[]) {

	if (argc < 2) return 1;

	doc = new TiXmlDocument(argv[1]);
	doc->LoadFile();

	//GLUTWindow* win = new GLUTWindow(640,480,"gldemo");
	//glutInit(&argc,argv);
	glutInitWindowSize(400,300);
	glutCreateWindow( "abc" );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// GL: enable and set colors
	glEnable( GL_COLOR_MATERIAL );
	glClearColor( 0.5, 0.5, 1.0, 1.0 );

	// GL: enable and set depth parameters
	glEnable( GL_DEPTH_TEST );
	glClearDepth( 1.0 );

	// GL: enable and set culling
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDisable( GL_CULL_FACE );
	//glCullFace(GL_BACK);

	// GL: light parameters
	GLfloat light_pos[] = { 0.0, 0.0, 1.0, 0.0 };
	GLfloat light_amb[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light_dif[] = { 0.9, 0.9, 0.9, 1.0 };

	// GL: enable lighting
	glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
	glLightfv( GL_LIGHT0, GL_AMBIENT,  light_amb );
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  light_dif );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

  // Die einzige sinnvolle BlendFunc fuer Transparenz
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

	// GL: misc stuff
	glShadeModel( GL_SMOOTH );
	glEnable( GL_NORMALIZE );

	// make functions known to GLUT
	glutDisplayFunc( display );
	glutKeyboardFunc( kbd );
	glutReshapeFunc( resize );
  glutMotionFunc(motion);
  glutMouseFunc(click);

	//win->run();
	glutMainLoop();

	return 0;
}

