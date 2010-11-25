#include "Pipeline2.h"
#include <GLUTWindow.h>

int width  = 640;
int height = 480;

GLUTWindow* win;
IntensityImage* tmp = 0;
Pipeline2* mypipe = 0;

int curframe = 0;
int frame = 0;
int lasttime = 0;
int lastframe = 0;

void disp() {

	int curtime = glutGet( GLUT_ELAPSED_TIME );
	if ((curtime - lasttime) >= 1000) {
		double fps = (1000.0*(curframe-lastframe))/((double)(curtime-lasttime));
		lasttime  = curtime;
		lastframe = curframe;
		std::cout << "fps: " << fps << std::endl;
	}

	win->clear( );
	win->mode2D();

	if (tmp) win->show( *tmp, 0, 0 );
	win->swap( );
}

void keyb( unsigned char c, int, int ) {

	if (c == 'q') exit(0);
	if (c == ' ') mypipe->reset();

	if (c == '1') tmp = (*mypipe)[0]->getImage();
	if (c == '2') tmp = (*mypipe)[1]->getImage();
	if (c == '3') tmp = (*mypipe)[2]->getImage();
	if (c == '4') tmp = (*mypipe)[3]->getImage();
	if (c == '5') tmp = (*mypipe)[4]->getImage();
	if (c == '6') tmp = (*mypipe)[5]->getImage();

	glutPostRedisplay();
}

void idle() {

	mypipe->process();
	if (curframe == 0) mypipe->reset();
	curframe++;

	glutPostRedisplay();
}

int main( int argc, char* argv[] ) {

	win = new GLUTWindow( width, height, "mypipeline test" );

	TiXmlDocument doc("test.xml");
	doc.LoadFile();

	mypipe = new Pipeline2( doc.FirstChildElement() );

	glutIdleFunc(idle);
	glutDisplayFunc(disp);
	glutKeyboardFunc(keyb);

	win->run();

}

