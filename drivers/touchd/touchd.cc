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
int showHelp = 0;
int editvalue = 0;
int storeSettingsAllFilter = 0;
int storeSettingsCurrentFilter = 0;
std::string userinput = "";

Pipeline* mypipe = 0;
std::string cfgfile;

int curframe = 0;
int lasttime = 0;
int lastframe = 0;
int angle = 0;

TUIOOutStream* tuio;


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

	// display data of configurator
	if(configure != 0) {

		configure->showInfo();

		if(showHelp == 1) {
			configure->showHelp();
		}

		if(editvalue == 1) {
			configure->showEditInfo();
		}

		if(storeSettingsAllFilter == 1) {
			configure->showStoreInfo(0);
		}
		else if(storeSettingsCurrentFilter == 1) {
			configure->showStoreInfo(1);
		}
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

	if (c == 'q') cleanup( 0 );
	if (c == ' ') mypipe->reset( 0 ); // reset all filter

	// switching to editing mode
	if (editvalue == 1 && configure != 0) {

		// quit edit mode without applying changes
		if(c == 0x1B) { // ESC
			editvalue = 0;
		}

		// Enter finishes Input
		if (c == 0x0D) {
			// parse input to double, 0.0 if a double couldn't be read
			double result = atof(userinput.c_str());
			// apply new value
			tmp->modifyOptionValue(result, true);
			std::cout << "input was: " << result << std::endl;
			editvalue = 0; // close editing mode
		} else {
			userinput += c;
		}

	}
	else if(storeSettingsAllFilter == 1 && configure != 0) {
		// save configuration of ALL filters

		// quit storing mode without saving
		if(c == 0x1B) { // ESC
			storeSettingsAllFilter = 0;
		}

		if(c == 0x0D){ // Enter
			// save new xml
			mypipe->storeXMLConfig(cfgfile);
			storeSettingsAllFilter = 0; // close storing mode
		}

	}
	else if(storeSettingsCurrentFilter == 1 && configure != 0) {
		// save configuration of CURRENT filter, keep all other settings

		// quit storing mode without saving
		if(c == 0x1B) { // ESC
			storeSettingsCurrentFilter = 0;
		}

		if(c == 0x0D) { // ENTER
			//(mypipe->storeXMLConfig(cfgfile); // TODO
			storeSettingsCurrentFilter = 0;
		}
	}
	else { // processing keyboard entries as usual
	// switching filters
		if ((c >= '0') && (c <= '9')) {
			c = c - '0';
			if (c < mypipe->size()){
				tmp = (*mypipe)[c];
				if(configure != 0) {
					configure->updateCurrentFilter(tmp);
				}
			}
		}

		// switch configurator on/off
		if(c == 'c'){
			if (configure == 0)
				configure = new Configurator(win, tmp);
			else{
				delete configure; // free memory, also calls destructor
				showHelp = 0;
				configure = 0;
			}
		}

		// adjust values
		if(configure != 0) {
			// show/hide help
			if(c == 'h') {
				showHelp = (showHelp + 1) % 2; // boolean value
			}

			// increase value
			if(c == 'i') {
				tmp->modifyOptionValue(1.0, false);
			}

			// decrease value
			if(c == 'd') {
				tmp->modifyOptionValue(-1.0, false);
			}

			// activate editing mode: overwrite non bool variables with user input
			if(c == 'e') {
				if(tmp->getOptionCount() > 0) {
					showHelp = 0;
					userinput = "";
					editvalue = 1; // boolean value
				}
			}

			// activate saving mode
			if(c == 's') { // store settings of CURRENT filter
				storeSettingsCurrentFilter = 1;
			}

			if(c == 'S') { // store settings of ALL filters
				storeSettingsAllFilter = 1;
			}

			// toggle Option with Tab
			if(c == 0x09) {
				tmp->nextOption();
			}

			// reset only this filter
			if(c == 'r') {
				tmp->reset( 0 );
			}
		}

		if(configure == 0) {
			// move Kinect only when configurator is closed
			if (c == 'x') {
				if( angle > -30 ) angle--;
				((Camera*)((*mypipe)[0]))->tilt_kinect( angle );
			}
			if (c == 'w') {
				if( angle < 30 ) angle++;
				((Camera*)((*mypipe)[0]))->tilt_kinect( angle );
			}
			if (c == 's') {
				angle = 0;
				((Camera*)((*mypipe)[0]))->tilt_kinect( angle );
			}
		}

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
	doc.LoadFile();

	// get Filter subtree
	TiXmlElement* root = doc.RootElement();
	
	// pass subtree of Filter subtree
	mypipe = new Pipeline( root );
	tmp = (*mypipe)[0]; // CameraFilter

	tuio = new TUIOOutStream( TISCH_TUIO1 | TISCH_TUIO2, address, outport );

	int width  = tmp->getImage()->getWidth();
	int height = tmp->getImage()->getHeight();

	if (!vidout) {
		while (1) idle();
	} else {
		win = new GLUTWindow( width, height, "libTISCH 2.0 image processor - press 0-9 to switch filter" );
		glutIdleFunc(idle);
		glutDisplayFunc(disp);
		glutKeyboardFunc(keyb);
		glutMouseFunc(mouse);
		win->run();
	}
}


