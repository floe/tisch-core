#include <signal.h>

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#include "Pipeline.h"
#include "BlobList.h"
#include <GLUTWindow.h>


int vidout  = 0;
int verbose = 0;

GLUTWindow* win = 0;
Filter* tmp = 0;

Pipeline2* mypipe = 0;
std::string cfgfile;

int curframe = 0;
int lasttime = 0;
int lastframe = 0;


#define OUTPUT_BUFFER_SIZE 0x10000
#define ADDRESS "127.0.0.1"
#define TISCH_PORT_RAW 3333

using namespace osc;

char buffer[OUTPUT_BUFFER_SIZE];
osc::OutboundPacketStream oscOut( buffer, OUTPUT_BUFFER_SIZE );

UdpTransmitSocket* transmitSocket; // ( IpEndpointName( ADDRESS, TISCH_PORT_RAW ) );


void cleanup( int signal ) {

	std::cout << "Cleaning up.. " << std::flush;

	delete transmitSocket;
	delete mypipe;

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
	win->swap();
}

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

	std::vector<int> alive;

	if (mypipe->process() != 0) curframe++;
	if (curframe == 2) mypipe->reset();

	oscOut << osc::BeginBundleImmediate;

	// frame message
	oscOut << osc::BeginMessage( "/tuio2/frm" ) << curframe << TimeTag(time(NULL)) << osc::EndMessage;

	// blob/pointer messages
	for (std::vector<Filter*>::iterator filter = mypipe->begin(); filter != mypipe->end(); filter++) {
		BlobList* bl = dynamic_cast<BlobList*>(*filter);
		if (bl) bl->send( oscOut, alive );
	}

	// alive message
	oscOut << osc::BeginMessage( "/tuio2/alv" );
	for (std::vector<int>::iterator id = alive.begin(); id != alive.end(); id++) oscOut << *id;
	oscOut << osc::EndMessage;

	transmitSocket->Send( oscOut.Data(), oscOut.Size() );
	oscOut.Clear();

	if (win) glutPostRedisplay();
}


int main( int argc, char* argv[] ) {

	std::cout << "touchd - libTISCH 2.0 image processing layer" << std::endl;
	std::cout << "(c) 2010 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	// create expected config file path
	const char* homedir = getenv( "HOME" ); if (!homedir) homedir = ".";
	cfgfile = std::string( homedir ) + "/.tisch.touchd.xml";

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "vVhdc:" ) ) switch (opt) {

		case 'v': verbose += 1; break;
		case 'V': vidout   = 1; break;

		case 'd': if (fork()) return 0; break;
		case 'c': cfgfile = optarg; break;

		case 'h':
		case '?': std::cout << "\nUsage: touchd [options]\n\n";
		          std::cout << "  -V          open video output window\n";
		          std::cout << "  -d          fork into background\n";
		          std::cout << "  -c cfgfile  use alternate config\n";
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

	mypipe = new Pipeline2( doc.FirstChildElement() );
	tmp = (*mypipe)[0];

	transmitSocket = new UdpTransmitSocket( IpEndpointName( ADDRESS, TISCH_PORT_RAW ) );

	int width  = tmp->getImage()->getWidth();
	int height = tmp->getImage()->getHeight();

	if (!vidout) {
		while (1) idle();
	} else {
		win = new GLUTWindow( width, height, "mypipeline test" );
		glutIdleFunc(idle);
		glutDisplayFunc(disp);
		glutKeyboardFunc(keyb);
		win->run();
	}
}

