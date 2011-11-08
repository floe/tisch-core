/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <nanolibc.h>
#include "Camera.h"

#ifdef HAS_DIRECTSHOW
	#include "DirectShowImageSource.h"
#endif

#ifdef HAS_FLYCAPTURE
	#include "FFMVImageSource.h"
#endif

#ifdef USE_BIGTOUCH
	#include "ledtouch/FlatSensorImageSource.h"
#endif

#ifdef __linux
	#include "V4LImageSource.h"
#endif

#ifdef HAS_FREENECT
	#include "KinectImageSource.h"
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


Camera::Camera( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {

	// generic low-end default settings
	width = 640; height = 480; fps = 30;
	sourcepath = "/dev/video0";
	useIntensityImage = 1;
	// setting variables for Configurator
	countOfOptions = 0; // quantity of variables that can be manipulated

	#ifdef __linux
		sourcetype = CAMERA_TYPE_V4L;
	#elif _MSC_VER
		sourcetype = CAMERA_TYPE_DIRECTSHOW;
	#else
		sourcetype = CAMERA_TYPE_DC1394;
	#endif

	// try to read settings from XML
	config->QueryIntAttribute   ( "SourceType", &sourcetype );
	config->QueryStringAttribute( "SourcePath", &sourcepath );
	config->QueryIntAttribute	( "UseIntensityImage", &useIntensityImage );

	config->QueryIntAttribute   ( "FlashMode",  &flashmode );
	config->QueryStringAttribute( "FlashPath",  &flashpath );

	config->QueryIntAttribute( "Width",  &width  );
	config->QueryIntAttribute( "Height", &height );
	config->QueryIntAttribute( "FPS",    &fps    );

	config->QueryIntAttribute( "Gain",       &gain   );
	config->QueryIntAttribute( "Exposure",   &expo   );
	config->QueryIntAttribute( "Brightness", &bright );

	config->QueryIntAttribute( "Verbose", &verbose );

	// create image buffer
	image = new IntensityImage( width, height, shmid, 1 );
	shortimage = new ShortImage( width, height );

	#ifdef HAS_DIRECTSHOW
		if (sourcetype == CAMERA_TYPE_DIRECTSHOW) 
			cam = new DirectShowImageSource( width, height, sourcepath.c_str(), verbose );
		else
	#endif

	#ifdef HAS_FLYCAPTURE
		if (sourcetype == CAMERA_TYPE_FFMV) 
			cam = new FFMVImageSource( width, height, sourcepath.c_str(), verbose );
		else
	#endif

	#ifdef __linux
		if (sourcetype == CAMERA_TYPE_V4L) 
			cam = new V4LImageSource( sourcepath, width, height, fps, verbose );
		else
	#endif

	#ifdef USE_BIGTOUCH
		if (sourcetype == CAMERA_TYPE_BIGTOUCH)
			cam = new FlatSensorImageSource( width, height, "bigtouch.bin", false ); // true );
		else
	#endif

	#ifdef HAS_FREENECT
		if (sourcetype == CAMERA_TYPE_KINECT) 
			cam = new KinectImageSource( );
		else
	#endif

	#ifdef HAS_DC1394
		if (sourcetype == CAMERA_TYPE_DC1394) {

			DCImageSource* dccam = new DCImageSource( width, height, fps, 0, verbose );
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
		/*
		* If the program crashes while debugging at this position please
		* check the specified SourceType in your configuration xml
		* have a look in the wiki for possible options
		*/
		throw std::runtime_error( "Error: unknown camera type requested. Linux "
				"needs SourceType=1, for Windows use SourceType=3" );

	// disable auto exposure, set other parameters
	cam->setExposure( IMGSRC_OFF );
	cam->setBrightness( bright );
	cam->setShutter( expo );
	cam->setGain( gain );

	#ifdef HAS_DC1394
		if (verbose) {
			cam->printInfo(DC1394_FEATURE_BRIGHTNESS);
			cam->printInfo(DC1394_FEATURE_EXPOSURE);
			cam->printInfo(DC1394_FEATURE_SHUTTER);
			cam->printInfo(DC1394_FEATURE_GAIN);
		}
	#endif

	// try to create a flash control instance
	try {
		flash = new FlashControl( flashpath.c_str() );
		flash->set( flashmode );
	} catch (...) {
		std::cout << "Warning: unable to control flash." << std::endl;
		flash = 0;
	}

	sleep(1); // FIXME: really needed for Kinect?
	cam->start();
}


Camera::~Camera() {

	cam->stop();

	delete flash;
	delete cam;
}

int Camera::process() {

	// get the image, retry on error
	int res = cam->acquire();
	if (!res) cam->acquire();

	// retrieve image, release buffer and return
	if(useIntensityImage) cam->getImage( *image );
#ifdef HAS_FREENECT
	else ((KinectImageSource*)cam)->getImage( *shortimage );
#endif
	cam->release();

	return 0;
}

void Camera::tilt_kinect( int angle ) {
#ifdef HAS_FREENECT
	if( sourcetype == CAMERA_TYPE_KINECT )
		((KinectImageSource*)cam)->tilt( angle );
#endif
}

TiXmlElement* Camera::getXMLRepresentation() {
		
	TiXmlElement* XMLNode = new TiXmlElement( "Camera" );

	XMLNode->SetAttribute( "SourceType", sourcetype );
	XMLNode->SetAttribute( "SourcePath", sourcepath );
	XMLNode->SetAttribute( "UseIntensityImage", useIntensityImage );

	XMLNode->SetAttribute( "Width", width );
	XMLNode->SetAttribute( "Height", height );
	XMLNode->SetAttribute( "FPS", fps );

	XMLNode->SetAttribute( "Verbose", verbose );

	XMLNode->SetAttribute( "FlashMode", flashmode );
	XMLNode->SetAttribute( "FlashPath", flashpath );

	XMLNode->SetAttribute( "Gain", gain );
	XMLNode->SetAttribute( "Exposure", expo );
	XMLNode->SetAttribute( "Brightness", bright);
	
	return XMLNode;
}
