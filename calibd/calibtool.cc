/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include <stdlib.h>

#include <nanolibc.h>

#include <signal.h>
#include <math.h>

#include "Calibration.h"
#include "GLUTWindow.h"
#include "UDPSocket.h"

#include "tisch.h"


#define MAX_CORR 4 // 8
#define HOM_CORR 4

#define OFFSET 20


// global objects
GLUTWindow* win = 0;
Calibration cal;

UDPSocket* input;

// total screen size
//Vector resolution( 800, 600 );
int xres = 800, yres = 600;

// current input candidates
std::map < int, std::vector < Vector > > blobs;
std::string target_id( "finger" );

// correspondences
std::vector< Vector > image_coords;
std::vector< Vector > screen_coords;
int current = 0;

std::string msg( "Please check corner markers and tap the cross.\nPress (f) for fullscreen, (q) to abort.\nNow sampling." );

int delay = 0;
char* myname = 0;
char** myargs;
int run = 2;
int fork_calibd = 1;



void handler( int signal ) { run = 0; }

void countdown( int signal ) {
	delay = 0;
	msg = "Please tap the cross.\nNow sampling.";
	glutPostRedisplay();
}

void cleanup() {

	std::cout << "Cleaning up.." << std::flush;

	delete input;
	delete win;

	std::cout << "done. Goodbye." << std::endl;

	if (fork_calibd) execv( "./calibd", myargs );

	exit( 0 );
}


void disp() {

	unsigned long int size, msglen = 0;
	int lxr = xres-1;
	int lyr = yres-1;

	win->clear();
	glColor4f(1.0,1.0,1.0,1.0);
	win->mode2D();

	glBegin(GL_LINES);
		
		// corner markers
		size = 40;
		glVertex2i(  0,  0); glVertex2i(  0,    size); glVertex2i(  0,  0); glVertex2i(    size,  0);
		glVertex2i(  0,lyr); glVertex2i(  0,lyr-size); glVertex2i(  0,lyr); glVertex2i(    size,lyr);
		glVertex2i(lxr,  0); glVertex2i(lxr,    size); glVertex2i(lxr,  0); glVertex2i(lxr-size,  0);
		glVertex2i(lxr,lyr); glVertex2i(lxr,lyr-size); glVertex2i(lxr,lyr); glVertex2i(lxr-size,lyr);

		// calibration cross
		if (!delay) {
			size = 15;
			double xoff = screen_coords[current].x;
			double yoff = screen_coords[current].y;
			glVertex2d(xoff-size,yoff);
			glVertex2d(xoff+size,yoff);
			glVertex2d(xoff,yoff-size);
			glVertex2d(xoff,yoff+size);
		}

	glEnd();

	msglen = msg.find("\n",0);
	if (msglen == std::string::npos) msglen = msg.length();

	win->print( msg.c_str(), (xres/2)-(4*msglen), (yres/2) );

	win->swap();
}


void keyb( unsigned char key, int x, int y ) {
	if ((key == 'f') || (key == 'F')) {
		glutPositionWindow( 0, 0 );
		glutReshapeWindow( xres, yres );
		glutFullScreen();
	}
	if ((key == 'q') || (key == 'Q')) {
		cal.restore( );
		std::cout << "User abort. Restarting.." << std::endl;
		run = 0;
	}
}


void idle() {

	if (run > 1) { run = 1; keyb( 'f', 0, 0 ); }
	if (!run) cleanup();

	std::string id;
	BasicBlob blob;

	(*input) >> id;

	if (!id.compare( "frame" )) {

		std::map< int, std::vector<Vector> >::iterator pos = blobs.begin();
		std::map< int, std::vector<Vector> >::iterator end = blobs.end();

		while ( pos != end ) {

			int erase = 0;
			int size = pos->second.size();

			if (size > 45) {

				Vector avg,avg2;

				for (int i = 0; i < size; i++) {
					Vector foo = pos->second[i];
					avg = avg + foo;
					foo.x *= foo.x;
					foo.y *= foo.y;
					avg2 = avg2 + foo;
				}

				avg  = avg  * (1.0/size);
				avg2 = avg2 * (1.0/size);

				Vector var( avg.x*avg.x, avg.y*avg.y );
				var = avg2 - var;

				if ((var.x > 1) || (var.y > 1)) erase = 1;
				else {
					// good enough..
					std::cout << "blob " << pos->first << " is persistent" << std::endl;
					std::cout << "average: " << avg << " deviation: " << var << std::endl;
					image_coords.push_back(avg);
					msg += "done.\nPlease remove your finger.";
					glutPostRedisplay();
					current++;
					delay = 1;
					alarm(3);
					blobs.clear();
					break;
				}
			}

			if (erase) blobs.erase(pos++);
			else ++pos;
		}

		// all correspondences captured?
		if (current == MAX_CORR) {

			cal.calculate( image_coords, screen_coords, HOM_CORR );
			cal.save( );

			std::cout << "New calibration saved. Restarting.." << std::endl;
			run = 0; return;
		}
	}

	if ((!input) || delay || id.compare( target_id )) { input->flush(); return; }
	(*input) >> blob; if (!input) { input->flush(); return; }
	blobs[blob.id].push_back(blob.peak);

	glutPostRedisplay();
}



int main( int argc, char* argv[] ) {

	std::cout << "calibtool - TISCH calibration layer 1.0 beta1" << std::endl;
	std::cout << "(c) 2008 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	myargs = argv;

	signal( SIGALRM, countdown );
	signal( SIGINT,  handler   );

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "fshq" ) ) switch (opt) {

		case 'f': target_id = "finger"; break;
		case 's': target_id = "shadow"; break;

		case 'q': fork_calibd = 0; break;

		case 'h':
		case '?':	std::cout << "Usage: calibtool [options]\n";
		          std::cout << "  -f  use finger tracking for calibration (default)\n";
		          std::cout << "  -s  use shadow tracking for calibration\n";
		          std::cout << "  -q  exit after calibration without forking calibd\n";
		          std::cout << "  -h  this\n";
		          return 0; break;
	}

	input = new UDPSocket( INADDR_ANY, TISCH_PORT_RAW );
	win = new GLUTWindow( xres, yres, "calibtool - TISCH calibration layer 1.0 beta1" );

	glutIdleFunc(idle);
	glutDisplayFunc(disp);
	glutKeyboardFunc(keyb);

	xres = glutGet( GLUT_SCREEN_WIDTH  );
	yres = glutGet( GLUT_SCREEN_HEIGHT );
	glutReshapeWindow( xres, yres );

	std::cout << "Using resolution: " << xres << "x" << yres << std::endl;

	// load & backup calibration to retrieve radial distortion parameters
	cal.load();
	cal.backup();

	// four corners: determine homography
	screen_coords.push_back( Vector(      OFFSET,      OFFSET ) );
	screen_coords.push_back( Vector( xres-OFFSET,      OFFSET ) );
	screen_coords.push_back( Vector( xres-OFFSET, yres-OFFSET ) );
	screen_coords.push_back( Vector(      OFFSET, yres-OFFSET ) );

	// four points midway between corners: determine radial distortion
	screen_coords.push_back( Vector(      xres/2,      OFFSET ) );
	screen_coords.push_back( Vector( xres-OFFSET,      yres/2 ) );
	screen_coords.push_back( Vector(      xres/2, yres-OFFSET ) );
	screen_coords.push_back( Vector(      OFFSET,      yres/2 ) );

	win->run();

	return 0;
}

