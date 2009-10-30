/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <assert.h>
#include <stdlib.h>

#include "Calibration.h"
#include "IntensityImage.h"
#include "RGBImage.h"
#include "GLUTWindow.h"
#include "Line.h"

#ifdef __linux
	#include "V4LImageSource.h"
	V4LImageSource* vsrc;
#endif
#ifdef HAS_DC1394
	#include "DCImageSource.h"
	DCImageSource* dsrc;
#endif

ImageSource* src;

#include <sys/time.h>

Calibration     cal;
GLUTWindow*     win;
IntensityImage* tmp;
IntensityImage* bla;

double coeff[4] = { 1.0, 0.0, 0.0, 0.0 };

int curframe = 0;
int frame = 0;
int mode = 0;

int do_set = 1;
int expo = 64;
int gain = 680;
int bright = 255;

int lasttime = 0;
int lastframe = 0;

int width = 640;
int height = 480;

Vector scale( width, width, 0 );
Vector delta( 0.5, (double)height/(2.0*width), 0 );


void disp() {

	int curtime = glutGet( GLUT_ELAPSED_TIME ); curframe++;
	if ((curtime - lasttime) >= 1000) {
		double fps = (1000.0*(curframe-lastframe))/((double)(curtime-lasttime));
		lasttime  = curtime;
		lastframe = curframe;
		if (src) std::cout << "fps: " << fps << std::endl;
	}

	win->clear( );

	if (mode) {
		std::ostringstream out;
		out << "Coefficients " << coeff[0] << ", " << coeff[1] << ", " << coeff[2] << ", " << coeff[3] << " (Press s to save.)";
		cal.set( coeff, delta, scale );
		tmp->undistort( scale, delta, coeff, *bla );
		win->show( *bla, 0, 0 );
		win->print(out.str(),10,10);
	} else {
		win->show( *tmp, 0, 0 );
		win->print("Press Space to switch to radial distortion mode.",10,10);
	}
	win->mode2D();

	win->swap( );
}

void special( int key, int, int ) {
	if (key == GLUT_KEY_UP   ) delta.y += 0.001;
	if (key == GLUT_KEY_DOWN ) delta.y -= 0.001;
	if (key == GLUT_KEY_LEFT ) delta.x += 0.001;
	if (key == GLUT_KEY_RIGHT) delta.x -= 0.001;
	glutPostRedisplay();
}

void keyb( unsigned char c, int, int ) {

	if (c == 's') cal.save();
	if (c == 'q') exit(1);
	if (c == ' ') mode = !mode;

	if (c == '(') { bright-=10; do_set = 1; }
	if (c == ')') { bright+=10; do_set = 1; }
	if (c == '-') { expo-=1; do_set = 1; }
	if (c == '+') { expo+=1; do_set = 1; }
	if (c == ',') { gain-=1; do_set = 1; }
	if (c == '.') { gain+=1; do_set = 1; }

	if (c == '1') coeff[0]-=0.01; 
	if (c == '2') coeff[0]+=0.01; 
	if (c == '3') coeff[1]-=0.01; 
	if (c == '4') coeff[1]+=0.01; 
	if (c == '5') coeff[3]-=0.01; 
	if (c == '6') coeff[3]+=0.01; 

	glutPostRedisplay();
}

void idle() {

	#ifdef HAS_DC1394
	if (do_set) {
		std::cout << "gain: " << gain << " shutter: " << expo << " brightness: " << bright << std::endl;
		dsrc->setBrightness(bright);
		dsrc->setShutter(expo);
		dsrc->setGain(gain);
		do_set = 0;
	}
	#endif

	src->acquire();
	src->getImage(*tmp);
	src->release();

	#ifdef HAS_DC1394
	if (frame++ % 90 == 45) {
		dsrc->printInfo(DC1394_FEATURE_GAIN);
		dsrc->printInfo(DC1394_FEATURE_SHUTTER);
		dsrc->printInfo(DC1394_FEATURE_EXPOSURE);
		dsrc->printInfo(DC1394_FEATURE_BRIGHTNESS);
	}
	#endif

	glutPostRedisplay();
}

int main( int argc, char* argv[] ) {


	try {

		std::cout << "distort - TISCH radial undistortion tool 1.0 rc1" << std::endl;
		std::cout << "(c) 2009 by Florian Echtler <echtler@in.tum.de>" << std::endl;
		
		bla = new IntensityImage( width, height );
		win = new GLUTWindow( width, height, "distort" );

		try {
			cal.load();
			cal.get( coeff, delta, scale );
		} catch(...) { }

		if (argc >= 2) {
			tmp = new IntensityImage( argv[1] );
		} else {
			tmp = new IntensityImage( width, height );
			#ifndef HAS_DC1394
				src = new V4LImageSource( "/dev/video0", width, height, 30, 1 );
			#else
				dsrc = new DCImageSource( width, height, 0, 0, 0 );

				dsrc->setExposure(IMGSRC_OFF);
	 
				dsrc->setBrightness(bright);
				dsrc->setShutter(expo);
				dsrc->setGain(gain);

				// switch GPIO0-3 to output
				int reg = dsrc->getReg( PIO_DIRECTION );
				dsrc->setReg( PIO_DIRECTION, reg | 0xF0000000 );

				// enable GPIO0-3 10us high strobe on exposure start
				dsrc->setReg( STROBE_0_CNT, 0x83000010 );
				dsrc->setReg( STROBE_1_CNT, 0x83000010 );
				dsrc->setReg( STROBE_2_CNT, 0x83000010 );
				dsrc->setReg( STROBE_3_CNT, 0x83000010 );

				/* // HDR Mode for Dragonfly
				#define HDR_CTRL 0x1800

				reg = dsrc->getReg( HDR_CTRL );
				dsrc->setReg( HDR_CTRL, reg | 0x02000000 );

				dsrc->setReg( HDR_CTRL+0x20, 0x82000190 );
				dsrc->setReg( HDR_CTRL+0x24, 0x8200005E );

				dsrc->setReg( HDR_CTRL+0x40, 0x820003DE );
				dsrc->setReg( HDR_CTRL+0x44, 0x820002AB );

				dsrc->setReg( HDR_CTRL+0x60, 0x82000190 );
				dsrc->setReg( HDR_CTRL+0x64, 0x8200005E );

				dsrc->setReg( HDR_CTRL+0x80, 0x820003DE );
				dsrc->setReg( HDR_CTRL+0x84, 0x820002AB );*/

				dsrc->printInfo();
				src = dsrc;
				sleep(1);

			#endif
			glutIdleFunc(idle);
		}

		glutDisplayFunc(disp);
		glutKeyboardFunc(keyb);
		glutSpecialFunc(special);

		win->run();

	} 
	catch (std::runtime_error& ex) {
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}

