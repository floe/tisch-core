/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _V4LIMAGESOURCE_H_
#define _V4LIMAGESOURCE_H_

#include <stdexcept> // runtime_error

#include "ImageSource.h"
#include "Image.h"


#define IMGSRC_BUFCOUNT 2


class V4LImageSource: public ImageSource {

	public:

		V4LImageSource( const std::string& path, int _width, int _height, int fps = 30, int debug = 0 );

		virtual ~V4LImageSource();

		void start();
		void stop();

		int  acquire();
		void release();

		void getImage( IntensityImage& target ) const;
		void getImage( RGBImage&       target ) const;

		void setGain( int gain );
		void setExposure( int exp );
		void setShutter( int speed );
		void setBrightness( int bright );

		void setFPS( int fps );

		void printInfo( int feature = 0 );

	private:

		void buf_ioctl( struct v4l2_buffer* buf, int num, int index, const char* name = 0 );

		// video device
		int vdev;

		// image parameters
		unsigned int format;
	
		// buffer management
		int bufcount, current;
		Image* buffers[IMGSRC_BUFCOUNT];

		// frame timing
		unsigned long long int frametime;
};

#endif // _V4LIMAGESOURCE_H_

