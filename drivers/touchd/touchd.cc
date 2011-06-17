/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler, <floe@butterbrot.org>   *
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
int startup = 1;

GLUTWindow* win = 0;
Filter* tmp = 0;
Configurator* configure = 0;
int showHelp = 0;
int editvalue = 0;
int storeconfig = 0;
std::string userinput = "";

Pipeline* mypipe = 0;
std::string cfgfile;
// TODO remove outputconfig
std::string outputconfig = "../drivers/touchd/output.xml";

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

TiXmlElement* getXMLSubTree(int startIndex, Filter* parentOfRoot) {
	
	TiXmlElement* rootOfCurrentSubtree = 0;

	if(startIndex < mypipe->size()) {
		// get XML Node for current root of subtree
		rootOfCurrentSubtree = (*mypipe)[startIndex]->getXMLRepresentation();
		
		// check pipe for further children of current root
		for(int i = startIndex + 1; i < mypipe->size(); i++) {

			// get parent of current filter
			Filter* parentOfCurrent = (*mypipe)[i]->getParent();

			// if parent of current == rootOfCurrentSubtree
			if(parentOfCurrent == (*mypipe)[startIndex]) {
				// then current is child of root
				rootOfCurrentSubtree->LinkEndChild(getXMLSubTree(i, (*mypipe)[i]));
			}
		}
	}

	return rootOfCurrentSubtree;
}

void storeXMLConfig() {
	// store filter settings
	std::cout << "storing XML Config" << std::endl;

	TiXmlDocument doc;
	//TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "");
	//doc.LinkEndChild(decl);
	
	TiXmlElement* tree = getXMLSubTree(0, 0);
	
	// add tree to document
	doc.LinkEndChild(tree);

	// save document to file
	doc.SaveFile(outputconfig); // cfgfile replace when outputconfig is removed
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

		if(storeconfig == 1) {
			configure->showStoreInfo();
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
	if (c == ' ') mypipe->reset();

	// switching to editing mode
	if (editvalue == 1 && configure != 0) {

		// quit edit mode without applying changes
		if(c == 'e') {
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
	else if(storeconfig == 1 && configure != 0) {
		// store current configuration

		// quit store mode without saving
		if(c == 'e') {
			storeconfig = 0;
		}

		if(c = 0x0D){ // Enter
			// save new xml
			storeXMLConfig();
			storeconfig = 0; // close storing mode
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
			if(c == 's') {
				storeconfig = 1;
			}

			// toggle Option with Tab
			if(c == 0x09) {
				tmp->nextOption();
			}

			//reset filter
			if(c == 'r') {
				tmp->reset();
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

	int outport = TISCH_PORT_RAW;

	std::cout << "touchd - libTISCH 2.0 image processing layer" << std::endl;
	std::cout << "(c) 2010 by Florian Echtler <floe@butterbrot.org>" << std::endl;

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

	mypipe = new Pipeline( doc.FirstChildElement() );
	tmp = (*mypipe)[0];

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


