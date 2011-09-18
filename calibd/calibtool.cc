/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
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
#include "TUIOInStream.h"
#include "Thread.h"

#include "tisch.h"


#define MAX_CORR 4 // 8
#define HOM_CORR 4

int     TARGET_OFFSET = 20;
#define TARGET_SIZE     40

#define MAX_VARIANCE 0.01
#define MIN_SAMPLES  50


// global objects
GLUTWindow* win = 0;
Calibration cal;

// total screen size
int xres = 0, yres = 0;

// current input candidates
std::map < int, std::vector < Vector > > blobs;

// correspondences
std::vector< Vector > image_coords;
std::vector< Vector > screen_coords;
int current = 0;

std::string msg( "Please check corner markers and tap the cross.\nPress (f) for fullscreen, (q) to abort.\nNow sampling." );

int delay = 0;
int ttype = INPUT_TYPE_FINGER;


void countdown( int signal ) {
	delay = 0;
	msg = "Please tap the cross.\nNow sampling.";
	glutPostRedisplay();
}

void disp() {

	unsigned long int size, msglen = 0;
	int lxr = xres-1; int mxr = xres/2;
	int lyr = yres-1; int myr = yres/2;

	win->clear();
	glColor4f(1.0,1.0,1.0,1.0);
	win->mode2D();

	glBegin(GL_LINES);
		
		size = TARGET_SIZE;

		// corner markers
		glVertex2i(  0,  0); glVertex2i(  0,    size); glVertex2i(  0,  0); glVertex2i(    size,  0);
		glVertex2i(  0,lyr); glVertex2i(  0,lyr-size); glVertex2i(  0,lyr); glVertex2i(    size,lyr);
		glVertex2i(lxr,  0); glVertex2i(lxr,    size); glVertex2i(lxr,  0); glVertex2i(lxr-size,  0);
		glVertex2i(lxr,lyr); glVertex2i(lxr,lyr-size); glVertex2i(lxr,lyr); glVertex2i(lxr-size,lyr);

		// side markers
		glVertex2i(   0, myr-size ); glVertex2i(   0, myr+size );
		glVertex2i( lxr, myr-size ); glVertex2i( lxr, myr+size );
		glVertex2i( mxr-size,   0 ); glVertex2i( mxr+size,   0 );
		glVertex2i( mxr-size, lyr ); glVertex2i( mxr+size, lyr );

		// calibration cross
		if (!delay) {
			size = (TARGET_SIZE/2)-5;
			double xoff = screen_coords[current].x * xres;
			double yoff = screen_coords[current].y * yres;
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

struct CalibTUIOInput: public TUIOInStream {

	CalibTUIOInput(): TUIOInStream( TISCH_PORT_RAW ) { }
	
	virtual void process_frame() {

		std::map< int, std::vector<Vector> >::iterator pos = blobs.begin();
		std::map< int, std::vector<Vector> >::iterator end = blobs.end();

		// calculate average position and std. deviation (= jitter)
		while ( pos != end ) {

			int erase = 0;
			int size = pos->second.size();

			if (size%10 == 0) { 
				msg += ".";
				glutPostRedisplay();
			}

			if (size > MIN_SAMPLES) {

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

				if ((var.x > MAX_VARIANCE) || (var.y > MAX_VARIANCE)) erase = 1;
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

			std::cout << "New calibration saved. Exiting..." << std::endl;
			exit(0);	
		}
	}

	virtual void process_blob( BasicBlob& b ) {
		if (!delay && (b.type == ttype))
			blobs[b.id].push_back(b.peak);
	}

};

CalibTUIOInput* input;

struct InputThread: public Thread {
	InputThread(): Thread() { }
	void* run() { input->run(); return 0; }
};

InputThread inputthread;


void keyb( unsigned char key, int x, int y ) {
	if ((key == 'f') || (key == 'F')) {
		glutPositionWindow( 0, 0 );
		glutReshapeWindow( xres, yres );
		glutFullScreen();
	}
	if ((key == 'q') || (key == 'Q')) {
		cal.restore( );
		std::cout << "User abort. Exiting..." << std::endl;
		input->stop();
		exit(1);
	}
}



int main( int argc, char* argv[] ) {

	bool circle = false;

	std::cout << "calibtool - libTISCH 2.0 calibration layer" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	signal( SIGALRM, countdown );

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "chx:y:t:o:" ) ) switch (opt) {

		case 'c': circle = true; break;

		case 'x': xres = atoi(optarg); break;
		case 'y': yres = atoi(optarg); break;

		case 't': ttype = atoi(optarg); break;
		case 'o': TARGET_OFFSET = atoi(optarg); break;

		case 'h':
		case '?': std::cout << "Usage: calibtool [options]\n";
		          std::cout << "  -x res  force x screen resolution\n";
		          std::cout << "  -y res  force y screen resolution\n";
		          std::cout << "  -o px   calibration point offset from screen border in pixels (default 20)\n";
		          std::cout << "  -c      assume circular display (centered)\n";
		          std::cout << "  -t num  use TUIO type #num for calibration, default = 1 (generic finger)\n";
		          std::cout << "  -h      this\n";
		          return 0; break;
	}

	input = new CalibTUIOInput();
	win = new GLUTWindow( xres, yres, "calibtool - libTISCH 2.0 calibration layer" );

	glutDisplayFunc(disp);
	glutKeyboardFunc(keyb);

	xres = xres ? xres : glutGet( GLUT_SCREEN_WIDTH  );
	yres = yres ? yres : glutGet( GLUT_SCREEN_HEIGHT );
	glutReshapeWindow( xres, yres );

	std::cout << "Using resolution: " << xres << "x" << yres << std::endl;

	// load & backup calibration to retrieve radial distortion parameters
	cal.load();
	cal.backup();

	// four corners: determine homography
	if (!circle) {
		screen_coords.push_back( Vector(      TARGET_OFFSET,      TARGET_OFFSET ) );
		screen_coords.push_back( Vector( xres-TARGET_OFFSET,      TARGET_OFFSET ) );
		screen_coords.push_back( Vector( xres-TARGET_OFFSET, yres-TARGET_OFFSET ) );
		screen_coords.push_back( Vector(      TARGET_OFFSET, yres-TARGET_OFFSET ) );
	} else {
		int min = (xres < yres) ? xres : yres;
		screen_coords.push_back( Vector( (xres-min)/2+TARGET_OFFSET, yres/2 ) );
		screen_coords.push_back( Vector( xres/2, (yres+min)/2-TARGET_OFFSET ) );
		screen_coords.push_back( Vector( (xres+min)/2-TARGET_OFFSET, yres/2 ) );
		screen_coords.push_back( Vector( xres/2, (yres-min)/2+TARGET_OFFSET ) );
	}

	/*int offx[4] = { OFFSET, (xres-2*OFFSET)/3, (xres-2*OFFSET)*2/3, xres-OFFSET };
	int offy[4] = { OFFSET, (yres-2*OFFSET)/3, (yres-2*OFFSET)*2/3, yres-OFFSET };

	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
		screen_coords.push_back( Vector( offx[i], offy[j] ) );

	// five points midway between corners: determine radial distortion
	screen_coords.push_back( Vector(      xres/2,      OFFSET ) );
	screen_coords.push_back( Vector( xres-OFFSET,      yres/2 ) );
	screen_coords.push_back( Vector(      xres/2, yres-OFFSET ) );
	screen_coords.push_back( Vector(      OFFSET,      yres/2 ) );
	screen_coords.push_back( Vector(      xres/2,      yres/2 ) );*/

	// scale down to (1.0,1.0) reference frame
	for (unsigned int i = 0; i < screen_coords.size(); i++) {
		screen_coords[i].x /= (double)xres;
		screen_coords[i].y /= (double)yres;
	}

	inputthread.start();
	win->run();

	return 0;
}

