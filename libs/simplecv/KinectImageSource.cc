/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/


#include <sys/types.h>
#include <sys/stat.h>

#include <string.h> // memset
#ifdef __linux
#include <sys/ioctl.h>
#include <unistd.h> // close
#include <error.h>  // errno
#include <asm/types.h>
#include <sys/time.h>
#endif

#ifdef _MSC_VER
#include "usb.h"
#endif

#include <errno.h>  // errno

#include <cstdio>
#include <iostream> // cout
#include <string>

#include "KinectImageSource.h"


KinectImageSource::KinectImageSource( int debug ) {

	#ifdef _MSC_VER
		usb_init();
	#else
		libusb_init( NULL );
	#endif

	// open the device
	if (freenect_init( &f_ctx, NULL ) < 0)
		throw std::runtime_error( "freenect_init() failed." );

	if (freenect_open_device( f_ctx, &f_dev, 0 ) < 0) 
		throw std::runtime_error( "freenect_open_device() failed." );

	pthread_mutex_init( &kinect_lock, NULL );
	pthread_cond_init(  &kinect_cond, NULL );

	width  = 640;
	height = 480;
	
	curdb  =   0;
	curvb  =   0;

	fps    =  30;
	run    =   1;

	depthbuf[0] = new ShortImage( width, height );
	depthbuf[1] = new ShortImage( width, height );
	videobuf[0] = new RGBImage( width, height );
	videobuf[1] = new RGBImage( width, height );

	freenect_set_user( f_dev, this );
}

KinectImageSource::~KinectImageSource() {
	stop();
	pthread_join( kinect_thread, NULL );
	delete depthbuf;
	delete videobuf;
}


KinectImageSource* src = 0;

void depth_cb( freenect_device* dev, void* depth, uint32_t timestamp ) {
	pthread_mutex_lock( &(src->kinect_lock) );
	src->curdb = (src->curdb+1)%2;
	freenect_set_depth_buffer( dev, src->depthbuf[src->curdb]->getData() );
	pthread_cond_signal( &(src->kinect_cond) );
	pthread_mutex_unlock( &(src->kinect_lock) );
}

void rgb_cb( freenect_device* dev, void* rgb, uint32_t timestamp ) {
	pthread_mutex_lock( &(src->kinect_lock) );
	src->curvb = (src->curvb+1)%2;
	freenect_set_video_buffer( dev, src->videobuf[src->curvb]->getData() );
	pthread_cond_signal( &(src->kinect_cond) );
	pthread_mutex_unlock( &(src->kinect_lock) );
}

void* kinecthandler( void* arg ) {

	src = (KinectImageSource*)arg;

	freenect_set_depth_callback( src->f_dev, depth_cb );
	freenect_set_video_callback( src->f_dev, rgb_cb );

	freenect_frame_mode dmode = freenect_find_depth_mode( FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT );
	freenect_frame_mode vmode = freenect_find_video_mode( FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB   );

	freenect_set_depth_mode( src->f_dev, dmode ); // assert(dmode.bytes == src->depthbuf[0]->size())
	freenect_set_video_mode( src->f_dev, vmode ); // assert(vmode.bytes == src->videobuf[0]->size())

	freenect_set_depth_buffer( src->f_dev, src->depthbuf[src->curdb]->getData() );
	freenect_set_video_buffer( src->f_dev, src->videobuf[src->curvb]->getData() );

	freenect_start_depth( src->f_dev );
	freenect_start_video( src->f_dev );

	while (src->run) {
		int res = freenect_process_events( src->f_ctx );
		if (res != 0) throw std::runtime_error( "freenect_process_events() failed." );
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
	pthread_mutex_lock( &kinect_lock );
	pthread_cond_wait( &kinect_cond, &kinect_lock );
	return 1;
}

void KinectImageSource::release() {
	pthread_mutex_unlock( &kinect_lock );
}


void KinectImageSource::getImage( IntensityImage& target ) const {
	uint16_t* source = (uint16_t*)depthbuf[(curdb+1)%2]->getData();
	uint8_t*  targ = (uint8_t*)target.getData();
	for (int i = 0; i < width*height; i++) {
		// the depth sensor is actually 11 bits - 2047 == too near/too far
		uint8_t tmp = source[i] >> 3;
		targ[i] = 255 - tmp; // remapping: 0 = no data, 254 = very close
	}
}

void KinectImageSource::getImage( ShortImage& target ) const {
//	target = *depthbuf;
	uint16_t* source = (uint16_t*)depthbuf[(curdb+1)%2]->getData();
	uint16_t* targ = (uint16_t*)target.getData();
	for (int i = 0; i < width*height; i++) {
		// the depth sensor is actually 11 bits - 2047 == too near/too far
		targ[i] = (2047 - source[i]) << 5;
	}
}

void KinectImageSource::getImage( RGBImage& target ) const {
	target = *(videobuf[(curvb+1)%2]);
}


void KinectImageSource::setGain( int gain ) { }
void KinectImageSource::setExposure( int exp ) { } 
void KinectImageSource::setShutter( int speed ) { }
void KinectImageSource::setBrightness( int bright ) { }
void KinectImageSource::printInfo( int feature ) { }
void KinectImageSource::setFPS( int fps ) { }

void KinectImageSource::tilt( int angle )
{
	freenect_set_tilt_degs(f_dev,angle);
}
