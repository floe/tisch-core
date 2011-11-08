/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _DCIMAGESOURCE_H_
#define _DCIMAGESOURCE_H_

#include "ImageSource.h"

#include <dc1394/control.h>
#include <dc1394/conversions.h>


// internal parameters
#define NUM_DMA_BUFFERS 8
#define SAFETY_DELAY 500

// some useful IIDC registers
#define FRAME_INFO    0x12F8
#define PIO_DIRECTION 0x11F8

#define STROBE_0_CNT  0x1500
#define STROBE_1_CNT  0x1504
#define STROBE_2_CNT  0x1508
#define STROBE_3_CNT  0x150C

#define HDR_CTRL      0x1800
#define HDR_SHUTTER0  HDR_CTRL+0x20
#define HDR_GAIN0     HDR_CTRL+0x24
#define HDR_SHUTTER1  HDR_CTRL+0x40
#define HDR_GAIN1     HDR_CTRL+0x44
#define HDR_SHUTTER2  HDR_CTRL+0x60
#define HDR_GAIN2     HDR_CTRL+0x64
#define HDR_SHUTTER3  HDR_CTRL+0x80
#define HDR_GAIN3     HDR_CTRL+0x84


class DCImageSource: public ImageSource {

	public:

		DCImageSource( int _width, int _height, int _fps = 0, int _num = 0, int _verbose = 0 );

		virtual ~DCImageSource();

		void start();
		void stop();

		int  acquire( );
		void release( );

		void getImage( IntensityImage& target ) const;
		void getImage( RGBImage&       target ) const;

		void setBrightness( int bright );
		void setExposure( int expo );
		void setShutter( int speed );
		void setGain( int gain );
		void setFPS( int fps );

		void printInfo( int feature = 0 );

		uint32_t getReg( uint64_t offset );
		void setReg( uint64_t offset, uint32_t value );

	protected:

		// helper functions
		void camera_setup( );
		void set_feature( dc1394feature_t feature, int value );

		// camera data structures
		dc1394_t* context;
		dc1394camera_list_t* list;
		dc1394camera_t** camera;
		dc1394featureset_t features;
		int verbose,num;

		// frame data
		dc1394video_frame_t* frame;
		unsigned char* rawdata;
		unsigned int framecount;
		unsigned long long int frametime;
};

#endif // _DCIMAGESOURCE_H_

