/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <nanolibc.h>

#include <signal.h>
#include <fcntl.h>
#include <math.h>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <Socket.h>

#include "Pipeline.h"
#include "touchd.h"
#include "tisch.h"

#include <time.h>

#define ADDRESS "127.0.0.1"

using namespace osc;

char buffer[OUTPUT_BUFFER_SIZE];
osc::OutboundPacketStream oscOut( buffer, OUTPUT_BUFFER_SIZE );

UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, TISCH_PORT_RAW ) );

// command line flags
int mode = TOUCHD_MODE_MIXED;
int verbose = 0;
int vidout = 0;

// window information strings
std::string title( "touchd (press 1-5 to switch channel, f/s to switch mode, q to quit)" );
std::string info( "" );
std::string cfgpath;

// helper objects
UDPSocket* out = 0;
GLUTWindow* win = 0;
Camera* cam = 0;

// image processing pipelines
Pipeline* finger = 0;
Pipeline* shadow = 0;

// video and pipeline parameters
// width height fps type startgain startexpo startbright startflash
#if defined(_MSC_VER)
	VideoSettings vidset = { 640, 480, 30, CAMERA_TYPE_DIRECTSHOW, 16, 260, 5, FLASH_MODE_AUTO };
#elif defined(USE_BIGTOUCH) 
	VideoSettings vidset = { 640, 480, 30, CAMERA_TYPE_BIGTOUCH, 16, 260, 5, FLASH_MODE_AUTO };
#elif defined(HAS_DC1394)
	VideoSettings vidset = { 752, 480,  0, CAMERA_TYPE_DC1394, 127, 8, 255, FLASH_MODE_AUTO };
#else
	VideoSettings vidset = { 640, 480, 30, CAMERA_TYPE_V4L, 16, 260, 5, FLASH_MODE_AUTO };
#endif

PipelineParameters fingerparm = {
	0, 0,
	{
		{ 0, 0, 0, FLASH_MODE_AUTO }, // apply gain expo flash
		{ 10, 0, 7, 0.01 },  // threshold invert noise bgfactor
		{ 20, 600, 1, 1.4, 15, 0.0, Color(1,0,0), Color(0,0,1) }, // minsize maxsize gid factor radius peakdist
		&vidset,
		"finger"
	}
};

PipelineParameters shadowparm = {
	0, 0,
	{
		{ 0, 0, 0, FLASH_MODE_AUTO }, // apply gain expo flash
		{ 10, 1, 2, 0.01 },  // threshold invert noise bgfactor
		{ 1000, 0, 1, 2.0, 30, 10000, Color(0,1,0), Color(0,0,1) }, // minsize maxsize gid factor radius peakdist
		&vidset,
		"shadow",
	}
};


// misc variables
int writeimg = 0;
int polarity = 1;
int frame = 0;
int setbg = 10;

Pipeline* current = 0;
int imgnum = 1;


// signal handling
int run = 1;
int img = 0;


void sigquit( int signum ) { run = 0; }
void sigclk ( int signum ) { img++;   }

/* 25 Hz clock to synchronize output video data
struct itimerval myclock = { { 0, 40000 }, { 0, 40000 } };
struct itimerval stop    = { { 0,     0 }, { 0,     0 } };*/

// glut callbacks
void keyb( unsigned char key, int x, int y ) {

	switch (key) {

		case '1': case '2': case '3': case '4': case '5':
			imgnum = key - 0x30; break;

		case 'f': if (mode & TOUCHD_MODE_FINGER) current = finger; break;
		case 's': if (mode & TOUCHD_MODE_SHADOW) current = shadow; break;

		case 'x': polarity = !polarity; break;
		case ' ': setbg = 1; break;
		case 'q': run = 0; break;
		case 'Q': run = 0; break;

		/*case 'w':

			if (!writeimg) {
				setitimer( ITIMER_REAL, &myclock, 0 );
				writeimg = 1;
			} else {
				setitimer( ITIMER_REAL, &stop,    0 );
				writeimg = 0;
				img = ((img/100000)+1)*100000;
			}

			break;*/
	}
}


void disp() {

	win->clear( );
	current->draw( win, imgnum );

	finger->draw( win, 0 );
	shadow->draw( win, 0 );

	glColor4f( 1.0, 0.0, 0.0, 1.0 );
	win->print( info, 5, 20 );
	win->title( title );

	win->swap();
}


void cleanup() {

	std::cout << "Cleaning up.. " << std::flush;

	std::ofstream config( cfgpath.c_str(), std::ios::trunc );
	if (config) {
		config << fingerparm.settings;
		config << shadowparm.settings;
	}

	delete out;
	delete cam;

	delete finger;
	delete shadow;

	if (vidout) delete win;

	std::cout << "done. Goodbye." << std::endl;
}


void idle() {

	int fint, sint;
	unsigned long long int ftime, stime;

	// image brightness sums
	// check for external signals
	if (!run) { cleanup(); exit(0); } //throw std::runtime_error( "User abort." ); if (frame == 500) exit(0);

	int curtime1 = glutGet( GLUT_ELAPSED_TIME );

	// get images from camera
	switch (mode) {

		case TOUCHD_MODE_FINGER: finger->acquire( fint, ftime ); break;
		case TOUCHD_MODE_SHADOW: shadow->acquire( sint, stime ); break;

		case TOUCHD_MODE_MIXED:

			if (polarity) {
				finger->acquire( fint, ftime );
				shadow->acquire( sint, stime );
			} else {
				shadow->acquire( sint, stime );
				finger->acquire( fint, ftime );
			}

			// check for completely lost frames
			unsigned long long int timediff = labs((long)(stime - ftime));
			int count = (int)round((double)timediff/16399087.25); // expected frame time difference in nanoseconds
			if ((count > 1) && (count < 2800)) { // 
				std::cout << "Warning: lost " << count-1 << " image(s)." << std::endl;
				if (count%2 == 0) return;
			}

			// check image order
			if (fint > sint) {
				if (verbose) std::cout << "WARNING: flipping image order. shadow int: " << sint << " finger int: " << fint << std::endl;
				finger->swap( shadow );
				//polarity = !polarity;
				//setbg = 1;
			}

			break;
	}

	int curtime2 = glutGet( GLUT_ELAPSED_TIME );

	// update background if forced
	if (setbg) {
		finger->update();
		shadow->update();
		setbg--;
		return;
	}

	TimeTag current_time = TimeTag(time(NULL));
	// announce new frame
//	(*out) << "frame " << ++frame << std::endl;
	oscOut  << osc::BeginBundleImmediate;
	oscOut	<< osc::BeginMessage( "/tuio2/frm" )
			<< ++frame
			<< current_time
			<< osc::EndMessage;

	// find blobs
	if (mode & TOUCHD_MODE_FINGER) finger->process();
	if (mode & TOUCHD_MODE_SHADOW) shadow->process();

	// if both modes are active, correlate fingers and shadows
	if (mode == TOUCHD_MODE_MIXED) finger->correlate( shadow );

	// send the lot
	if (mode & TOUCHD_MODE_FINGER) finger->send(oscOut);
	if (mode & TOUCHD_MODE_SHADOW) shadow->send(oscOut);
	
	transmitSocket.Send( oscOut.Data(), oscOut.Size() );
	oscOut.Clear();

	if (verbose && ((frame % (vidset.fps?vidset.fps:60)) == 0)) std::cout << "acquisition: " << (curtime2-curtime1) << "ms. processing: " << (glutGet(GLUT_ELAPSED_TIME)-curtime2) << " ms." << std::endl;

	// redraw window
	if (vidout) glutPostRedisplay();

	/*if (!writeimg) { info = ""; return; }

	// get the frame number once
	info = "video output on";
	int imgtmp = img;

	{
		IntensityImage tmp( *(finger->binary), 1 );
		std::ostringstream name0;
		name0 << "log/finger" << std::setfill('0') << std::setw(6) << imgtmp << ".pgm";
		std::ofstream f0( name0.str().c_str(), std::ios::trunc );
		f0 << tmp; f0.close();
	}

	{
		IntensityImage tmp( *(shadow->binary), 1 );
		std::ostringstream name1;
		name1 << "log/shadow" << std::setfill('0') << std::setw(6) << imgtmp << ".pgm";
		std::ofstream f1( name1.str().c_str(), std::ios::trunc );
		f1 << tmp; f1.close();
	}*/
}



int main( int argc, char* argv[] ) {

	std::cout << "touchd - libTISCH 1.1 image processing layer" << std::endl;
	std::cout << "(c) 2008 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	// create expected config file path
	char* homedir = getenv( "HOME" ); if (!homedir) homedir = "/tmp";
	cfgpath = std::string( homedir ) + "/.tisch.touchd";

	std::ifstream config( cfgpath.c_str() );
	if (config) {
		config >> fingerparm.settings;
		config >> shadowparm.settings;
		if (!config) throw std::runtime_error( "Config file parsing error." );
	}

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "msfvVhdw" ) ) switch (opt) {

		//case 'w': writeimg = 1; setitimer( ITIMER_REAL, &myclock, 0 ); break;

		case 'f': mode = TOUCHD_MODE_FINGER; vidset.startflash = FLASH_MODE_FINGER; break;
		case 's': mode = TOUCHD_MODE_SHADOW; vidset.startflash = FLASH_MODE_SHADOW; break;
		case 'm': mode = TOUCHD_MODE_MIXED;  vidset.startflash = FLASH_MODE_AUTO;   break;

		case 'v': verbose += 1; break;
		case 'V': vidout   = 1; break;

		case 'd': if (fork()) return 0; break;

		case 'h':
		case '?': std::cout << "\nUsage: touchd [options]\n\n";
		          std::cout << "  -m  use mixed tracking mode\n";
		          std::cout << "  -f  use only finger tracking\n";
		          std::cout << "  -s  use only shadow tracking\n";
		          std::cout << "  -V  open video output window\n";
		          std::cout << "  -d  fork into background\n";
		          //std::cout << "  -w  write video stream\n";
		          std::cout << "  -v  be verbose\n";
		          std::cout << "  -h  this\n\n";
		          return 0; break;
	}

	signal( SIGQUIT, sigquit );
	signal( SIGTERM, sigquit );
	signal( SIGINT,  sigquit );
	signal( SIGALRM, sigclk  );

	try {

		out = new UDPSocket( INADDR_ANY, 0, 0, (verbose>1) ); out->target( INADDR_LOOPBACK, TISCH_PORT_RAW );
		cam = new Camera( &vidset, verbose );

		fingerparm.camera = shadowparm.camera = cam;
		fingerparm.output = shadowparm.output = out;

		finger = new Pipeline( &fingerparm ); if (mode & TOUCHD_MODE_FINGER) current = finger;
		shadow = new Pipeline( &shadowparm ); if (mode & TOUCHD_MODE_SHADOW) current = shadow;

		if (!vidout) {

			while (1) idle();

		} else {

			win = new GLUTWindow( vidset.width, vidset.height, title );

			glutIdleFunc(idle);
			glutDisplayFunc(disp);
			glutKeyboardFunc(keyb);

			win->run();

		}

	} catch (std::runtime_error& err) {
		std::cerr << "Caught runtime exception:\n  " << err.what() << std::endl;
		cleanup();
		return 1;
	}

	return 0;
}

