/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Camera.h"

#ifdef _MSC_VER
	#include "DirectShowImageSource.h"
#endif

#if defined(USE_BIGTOUCH)
	#include "ledtouch/FlatSensorImageSource.h"
#endif

#ifdef __linux
	#include "V4LImageSource.h"
#endif

#ifdef HAS_DC1394
	#include "DCImageSource.h"
#endif


/*
#include <fstream>
#include <sstream>

void diff_1st( int in[], int out[] ) { out[0] = out[255] = 0; for (int i = 1; i < 255; i++) out[i] = (in[i+1] - in[i-1])/2; }
void smooth( int in[], int out[] ) {
	out[ 0 ] = ( in[ 0 ] + in[ 1 ] + in[ 2 ]           ) / 3;
	out[ 1 ] = ( in[ 0 ] + in[ 1 ] + in[ 2 ] + in[ 3 ] ) / 4;
	out[254] = ( in[255] + in[254] + in[253] + in[252] ) / 4;
	out[255] = ( in[255] + in[254] + in[253]           ) / 3;
	for (int i = 2; i < 254; i++) out[i] = ( in[i-2] + in[i-1] + in[i] + in[i+1] + in[i+2] ) / 5;
}

void dump_plot( int in[], const char* name ) {
	std::ofstream f0( name, std::ios::trunc );
	f0 << "set term x11 persist\nset grid\nset xrange [0:256]\nset yrange [-5000:5000]\nplot '-' with linespoints\n";
	for (int i = 0; i < 256; i++) f0 << i << " " << in[i] << std::endl;
	f0.close();
}*/


Camera::Camera( VideoSettings* vidset, int verbose, const char* videodev, const char* ctrlport ) {

	#ifdef _MSC_VER
		if (vidset->source == CAMERA_TYPE_DIRECTSHOW) 
			cam = new DirectShowImageSource( vidset->width, vidset->height, videodev, verbose );
		else
	#endif

	#ifdef __linux
		if (vidset->source == CAMERA_TYPE_V4L) 
			cam = new V4LImageSource( videodev, vidset->width, vidset->height, vidset->fps, verbose );
		else
	#endif

	#if defined(USE_BIGTOUCH)
		if (vidset->source == CAMERA_TYPE_BIGTOUCH)
			cam = new FlatSensorImageSource( vidset->width, vidset->height, "bigtouch.bin", false ); // true );
		else
	#endif

	#ifdef HAS_DC1394
		if (vidset->source == CAMERA_TYPE_DC1394) {

			DCImageSource* dccam = new DCImageSource( vidset->width, vidset->height, vidset->fps, 0, verbose );
			cam = dccam;

			// switch GPIO0-3 to output
			int reg = dccam->getReg( PIO_DIRECTION );
			dccam->setReg( PIO_DIRECTION, reg | 0xF0000000 );

			// enable GPIO0-3 10us high strobe on exposure start
			dccam->setReg( STROBE_0_CNT, 0x83000020 );
			dccam->setReg( STROBE_1_CNT, 0x83000020 );
			dccam->setReg( STROBE_2_CNT, 0x83000020 );
			dccam->setReg( STROBE_3_CNT, 0x83000020 );

			// HDR Mode for Dragonfly
			/*reg = dccam->getReg( HDR_CTRL );
			dccam->setReg( HDR_CTRL, reg | 0x02000000 );

			dccam->setReg( HDR_SHUTTER0, 0x820002BC );
			dccam->setReg( HDR_GAIN0,    0x820002AB );

			dccam->setReg( HDR_SHUTTER1, 0x82000040 );
			dccam->setReg( HDR_GAIN1,    0x820002AB );

			dccam->setReg( HDR_SHUTTER2, 0x820002BC );
			dccam->setReg( HDR_GAIN2,    0x820002AB );

			dccam->setReg( HDR_SHUTTER3, 0x82000040 );
			dccam->setReg( HDR_GAIN3,    0x820002AB );*/
		} else
	#endif

		throw std::runtime_error( "Error: unknown camera type requested." );

	cam->setExposure( IMGSRC_OFF );

	cam->setBrightness( vidset->startbright );
	cam->setShutter( vidset->startexpo );
	cam->setGain( vidset->startgain );

	#ifdef HAS_DC1394
		if (verbose) {
			cam->printInfo(DC1394_FEATURE_BRIGHTNESS);
			cam->printInfo(DC1394_FEATURE_EXPOSURE);
			cam->printInfo(DC1394_FEATURE_SHUTTER);
			cam->printInfo(DC1394_FEATURE_GAIN);
		}
	#endif

	#if defined(USE_BIGTOUCH)
		cam->start();
	#endif

	try {
		flash = new FlashControl( ctrlport );
		flash->set( vidset->startflash );
	} catch (...) {
		std::cout << "Warning: unable to control flash." << std::endl;
		flash = 0;
	}
}


Camera::~Camera() {
	#if defined(USE_BIGTOUCH)
		cam->stop();
	#endif

	delete flash;
	delete cam;
}


void Camera::apply( CameraSettings* camset ) {
	// adjust camera settings once
	if (camset->apply) {
		cam->setShutter( camset->exposure );
		cam->setGain( camset->gain );
		flash->set( camset->flash );
		camset->apply = 0;
	}
}


void Camera::acquire( IntensityImage* target ) {

	// get the image, try again on error..
	int res = cam->acquire();
	if (!res) cam->acquire();

	// retrieve image, release buffer and return
	cam->getImage( *target );
	cam->release();
}

