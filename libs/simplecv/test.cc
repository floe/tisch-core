#include "V4LImageSource.h"
#include <GLUTWindow.h>

V4LImageSource* vsrc;
GLUTWindow* win;
RGBImage buf(640,480);

IntensityImage hue(640,480);
IntensityImage sat(640,480);
IntensityImage val(640,480);
IntensityImage max(640,480);

int mode = 0;

void display() {

	win->mode2D();
	win->clear(1.0,1.0,1.0);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	vsrc->acquire();
	vsrc->getImage(buf);
	vsrc->release();

	buf.getHSV(hue,sat,val);

	switch (mode%4) {
		case 0: break;
		case 1: buf.combine(hue,max,val); break;
		case 2: buf.combine(hue,sat,max); break;
		case 3: buf.combine(hue,max,max); break;
	}

	win->show(buf,0,0);

	glutSwapBuffers();
	glutPostRedisplay();
}

void kbd( unsigned char key, int, int ) {
	if (key == 'q') exit(1);
	if (key == ' ') mode++;
}

int main(int argc, char* argv[]) {

	win = new GLUTWindow(640,480,"cvtest");

	vsrc = new V4LImageSource("/dev/video0",640,480,30,1);
	vsrc->start();

	max.clear(255);

	// make functions known to GLUT
	glutDisplayFunc( display );
	glutKeyboardFunc( kbd );
	glutIdleFunc( 0 );

	win->run();

	return 0;
}

