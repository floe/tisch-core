/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _KINECTIMAGESOURCE_H_
#define _KINECTIMAGESOURCE_H_

#include <stdexcept> // runtime_error
#include <pthread.h>
#include <libfreenect.h>

#include "ImageSource.h"
#include "ShortImage.h"
#include "RGBImage.h"


class KinectImageSource: public ImageSource {

	public:

		KinectImageSource( int debug = 0 );

		virtual ~KinectImageSource();

		void start();
		void stop();

		int  acquire();
		void release();

		void getImage( IntensityImage& target ) const;
		void getImage( ShortImage& target ) const;
		void getImage( RGBImage&   target ) const;

		void setGain( int gain );
		void setExposure( int exp );
		void setShutter( int speed );
		void setBrightness( int bright );
		void setFPS( int fps );
		void printInfo( int feature = 0 );

		// video device
		freenect_context* f_ctx;
		freenect_device*  f_dev;

		// worker thread
		pthread_t kinect_thread;

		// thread control
		int run;

		// buffer management
		int bufcount, current;
		ShortImage* depthbuf;
		RGBImage* rgbbuf;

		pthread_mutex_t kinect_lock;
		pthread_cond_t  kinect_cond;
};

#endif // _KINECTIMAGESOURCE_H_

