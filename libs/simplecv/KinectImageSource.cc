/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> // close
#include <string.h> // memset
#include <error.h>  // errno
#include <errno.h>  // errno

#include <sys/time.h>
#include <asm/types.h>

#include <cstdio>
#include <iostream> // cout
#include <string>

#include "KinectImageSource.h"

extern "C" {
#include <libfreenect.h>
}


KinectImageSource::KinectImageSource( int debug ) {

	libusb_init( NULL );

	// open the device
	kdev = libusb_open_device_with_vid_pid( NULL, 0x045E, 0x02AE );
	if (kdev == 0) throw std::runtime_error( "Kinect USB device not found." );

	width  = 640;
	height = 480;
	fps    =  30;
	run    =   1;

	for (int i = 0; i < KINECT_BUFCOUNT; i++) {
		buffers[i] = new ShortImage( width, height );
		memset( buffers[i]->getData(), 128, width*height*2 );
	}

	start();
}

KinectImageSource::~KinectImageSource() {
	stop();
	pthread_join( kinect_thread, NULL );
}


KinectImageSource* src = 0;

void depth_cb( uint16_t* buf, int width, int height ) {
	memcpy( src->buffers[0]->getData(), buf, width*height*2 );
}

void rgb_cb( uint8_t* buf, int width, int height ) {
}

void* kinecthandler( void* arg ) {

	src = (KinectImageSource*)arg;
	cams_init( src->kdev, depth_cb, rgb_cb );

	while (src->run) {
		int res = libusb_handle_events( NULL );
		if (res != 0) throw std::runtime_error( "error in libusb_handle_event." );
	}

	return 0;
}


void KinectImageSource::start() {
	int res = pthread_create( &kinect_thread, NULL, kinecthandler, this );
	if (res != 0) throw std::runtime_error( "error in pthread_create." );
}

void KinectImageSource::stop() {
	run = 0;
}


int KinectImageSource::acquire() {
}

void KinectImageSource::release() {
}


void KinectImageSource::getImage( IntensityImage& target ) const {
	uint16_t* srce = (uint16_t*)buffers[0]->getData();
	uint8_t*  targ = (uint8_t*)target.getData();
	for (int i = 0; i < width*height; i++) {
		// the depth sensor is actually 11 bits - 2047 == too near/too far
		targ[i] = srce[i] >> 3;
	}
}

void KinectImageSource::getImage( ShortImage& target ) const {
	uint16_t* srce = (uint16_t*)buffers[0]->getData();
	uint16_t* targ = (uint16_t*)target.getData();
	for (int i = 0; i < width*height; i++) {
		// the depth sensor is actually 11 bits - 2047 == too near/too far
		targ[i] = srce[i] << 5;
	}
}

void KinectImageSource::getImage( RGBImage& target ) const { }


void KinectImageSource::setGain( int gain ) { }
void KinectImageSource::setExposure( int exp ) { } 
void KinectImageSource::setShutter( int speed ) { }
void KinectImageSource::setBrightness( int bright ) { }
void KinectImageSource::printInfo( int feature ) { }
void KinectImageSource::setFPS( int fps ) { }

