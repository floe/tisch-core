/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <tisch.h>
#include <signal.h>
#include <typeinfo>

#include <nanolibc.h>
#include "Pipeline.h"
#include "BlobList.h"
#include <GLUTWindow.h>
#include <TUIOOutStream.h>

#include "Camera.h"
#include "Configurator.h"


const char* address = "127.0.0.1";

int vidout  = 0;
int verbose = 0;
int startup = 5;

GLUTWindow* win = 0;
Filter* tmp = 0;
Configurator* configure = 0;
bool storeSettings = false;

Pipeline* mypipe = 0;
std::string cfgfile;

int curframe = 0;
int lasttime = 0;
int lastframe = 0;
int angle = 0;

int mode = FILTER_TYPE_SHORT;

TUIOOutStream* tuio;


void cleanup( int signal ) {

	std::cout << "Cleaning up.. " << std::flush;

	delete mypipe;
	delete tuio;

	if (vidout) {
		delete configure;
		delete win;
	}

	std::cout << "done. Goodbye." << std::endl;

	exit(0);
}

void disp() {

	int curtime = glutGet( GLUT_ELAPSED_TIME );
	if ((curtime - lasttime) >= 1000) {
		double fps = (1000.0*(curframe-lastframe))/((double)(curtime-lasttime));
		lasttime  = curtime;
		lastframe = curframe;
		std::cout << "fps: " << fps << std::endl;
	}

	win->clear();
	win->mode2D();

	tmp->draw( win, mode );

	// display data of configurator
	configure->draw();

	if (storeSettings) {
	
		int xCoord = 100;
		int yCoord = 140;

		glColor4f(1.0, 1.0, 1.0, 1.0); // white
		win->drawRectangleBackground(xCoord, yCoord, 450, 100, 2);

		glColor4f(0.0, 0.0, 0.0, 1.0); // black
		win->print(std::string("Saving Mode"), xCoord, yCoord);
		yCoord += 30;
		win->print(std::string("Press ENTER to save current settings of ALL filters."), xCoord, yCoord);
		yCoord += 30;
		win->print(std::string("Press any other key to leave without saving."), xCoord, yCoord);
	}

	win->swap();
}

void mouse( int button, int state, int x, int y )
{
	double imagex = ((double)x / win->getWidth()) * (tmp->getImage())->getWidth();
	double imagey = ((double)y / win->getHeight()) * (tmp->getImage())->getHeight();
	tmp->processMouseButton( button, state, imagex, imagey );
}

// TODO: better keyboard functionality (adjust filter params)
void keyb( unsigned char c, int, int ) {

	if (storeSettings) {
		if (c == 0x0D) mypipe->storeXMLConfig(cfgfile); // Enter
		storeSettings = false;
	}

	if (configure->handleInput(c)) return;

	if (c == 'q') cleanup( 0 );

	if (c == ' ') mypipe->reset( 0 ); // reset all filters
	if (c == 'r') tmp->reset( 0 ); // reset only this filter

	// switch between displaying DepthImage and RGBImage
	if (c == 'm')
		mode = (mode==FILTER_TYPE_SHORT) ? FILTER_TYPE_RGB : FILTER_TYPE_SHORT;

	// store settings of ALL filters
	if (c == 'S') 
		storeSettings = true;

	// switching filters
	if ((c >= '0') && (c <= '9')) {
		c = c - '0';
		if (c < mypipe->size()){
			tmp = (*mypipe)[c];
			configure->updateCurrentFilter(tmp);
		}
	}

	// move Kinect (if applicable)
	if (c == 'x') {
		if (angle > -30) angle--;
		((Camera*)((*mypipe)[0]))->tilt_kinect( angle );
	}
	if (c == 'w') {
		if (angle < 30) angle++;
		((Camera*)((*mypipe)[0]))->tilt_kinect( angle );
	}
	if (c == 's') {
		angle = 0;
		((Camera*)((*mypipe)[0]))->tilt_kinect( angle );
	}

	glutPostRedisplay();
}

void idle() {

	if (mypipe->process() == 0) curframe++;
	if (startup > 0) {
		mypipe->reset( 1 ); // initial Reset: yes!
		startup--;
	}

	tuio->start();

	// blob/pointer messages
	for (std::vector<Filter*>::iterator filter = mypipe->begin(); filter != mypipe->end(); filter++) {
		BlobList* bl = dynamic_cast<BlobList*>(*filter);
		if (bl) bl->send( tuio );
	}

	tuio->send();

	if (win) glutPostRedisplay();
}

int main( int argc, char* argv[] ) {

	int width,height;
	int outport = TISCH_PORT_RAW;

	std::cout << "touchd - libTISCH 2.0 image processing layer" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	// create expected config file path
	const char* homedir = getenv( "HOME" );
	if (!homedir || (homedir[0] == 0)) homedir = ".";
	cfgfile = std::string(homedir) + "/.tisch.touchd.xml";

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "vVhdc:p:t:" ) ) switch (opt) {

		case 'v': verbose += 1; break;
		case 'V': vidout   = 1; break;

		case 'd': if (fork()) return 0; break;
		case 'c': cfgfile = optarg; break;
		case 't': address = optarg; break;
		case 'p': outport = atoi(optarg); break;

		case 'h':
		case '?': std::cout << "\nUsage: touchd [options]\n\n";
		          std::cout << "  -t target   use alternate target host (default: 127.0.0.1)\n";
		          std::cout << "  -p udpport  use alternate target port (default: 3333)\n";
		          std::cout << "  -c cfgfile  use alternate config file\n";
		          std::cout << "  -V          open video output window\n";
		          std::cout << "  -d          fork into background\n";
		          std::cout << "  -v          be verbose\n";
		          std::cout << "  -h          this\n\n";
		          return 0; break;
	}

	signal( SIGQUIT, cleanup );
	signal( SIGTERM, cleanup );
	signal( SIGINT,  cleanup );

	std::cout << "Loading configuration from " << cfgfile << std::endl;
	TiXmlDocument doc( cfgfile );
	bool success = doc.LoadFile();

	if(!success) {
		std::cout << "no config file found at specified location!" << std::endl;
		std::cout << "quit!" << std::endl;
		return -1;
	}

	// get Filter subtree
	TiXmlElement* root = doc.RootElement();
	
	// pass subtree of Filter subtree
	mypipe = new Pipeline( root );
	tmp = (*mypipe)[0]; // CameraFilter

	tuio = new TUIOOutStream( TISCH_TUIO1 | TISCH_TUIO2, address, outport );

	if (tmp->getImage()) {
		width  = tmp->getImage()->getWidth();
		height = tmp->getImage()->getHeight();
	} else if (tmp->getShortImage()) {
		width  = tmp->getShortImage()->getWidth();
		height = tmp->getShortImage()->getHeight();
	} 

	if (!vidout) {
		while (1) idle();
	} else {
		win = new GLUTWindow( width, height, "libTISCH 2.0 image processor - press 0-9 to switch filter" );
		configure = new Configurator( win, tmp );
		glutIdleFunc(idle);
		glutDisplayFunc(disp);
		glutKeyboardFunc(keyb);
		glutMouseFunc(mouse);
		win->run();
	}
}

