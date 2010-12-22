#include <tisch.h>
#include <signal.h>
#include <typeinfo>

#include <nanolibc.h>
#include "Pipeline.h"
#include "BlobList.h"
#include <GLUTWindow.h>
#include <TUIOStream.h>

#define ADDRESS "127.0.0.1"


int vidout  = 0;
int verbose = 0;
int startup = 1;

GLUTWindow* win = 0;
Filter* tmp = 0;

Pipeline* mypipe = 0;
std::string cfgfile;

int curframe = 0;
int lasttime = 0;
int lastframe = 0;

TUIOStream* tuio;


void cleanup( int signal ) {

	std::cout << "Cleaning up.. " << std::flush;

	delete mypipe;
	delete tuio;

	if (vidout) delete win;

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

	tmp->draw( win );

	std::string filter = typeid( *tmp ).name();
	const char* name = filter.c_str();
	while (name && (*name >= '0') && (*name <= '9')) name++;

	glColor4f( 1.0, 0.0, 0.0, 1.0 );
	win->print( std::string("showing filter: ") + name, 10, 10 );

	win->swap();
}

// TODO: better keyboard functionality (adjust filter params)
void keyb( unsigned char c, int, int ) {

	if (c == 'q') cleanup( 0 );
	if (c == ' ') mypipe->reset();

	if ((c >= '0') && (c <= '9')) {
		c = c - '0';
		if (c < mypipe->size())
			tmp = (*mypipe)[c];
	}

	glutPostRedisplay();
}

void idle() {

	if (mypipe->process() == 0) curframe++;
	if (startup) { mypipe->reset(); startup = 0; }

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

	int outport = TISCH_PORT_CALIB;

	std::cout << "touchd - libTISCH 2.0 image processing layer" << std::endl;
	std::cout << "(c) 2010 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	// create expected config file path
#ifdef _MSC_VER
	std::string homedir = "";
#else
	std::string homedir = getenv( "HOME" );
#endif
	if (homedir == "") homedir = ".";
	cfgfile = homedir + "/.tisch.touchd.xml";

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "vVhdc:p:" ) ) switch (opt) {

		case 'v': verbose += 1; break;
		case 'V': vidout   = 1; break;

		case 'd': if (fork()) return 0; break;
		case 'c': cfgfile = optarg; break;
		case 'p': outport = atoi(optarg); break;

		case 'h':
		case '?': std::cout << "\nUsage: touchd [options]\n\n";
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
	doc.LoadFile();

	mypipe = new Pipeline( doc.FirstChildElement() );
	tmp = (*mypipe)[0];

	tuio = new TUIOStream( ADDRESS, outport );

	int width  = tmp->getImage()->getWidth();
	int height = tmp->getImage()->getHeight();

	if (!vidout) {
		while (1) idle();
	} else {
		win = new GLUTWindow( width, height, "libTISCH 2.0 image processor - press 0-9 to switch filter" );
		glutIdleFunc(idle);
		glutDisplayFunc(disp);
		glutKeyboardFunc(keyb);
		win->run();
	}
}

