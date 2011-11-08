/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _IMAGESOURCE_H_
#define _IMAGESOURCE_H_

#include "RGBImage.h"

// special parameters for camera features
#define IMGSRC_OFF  -12345
#define IMGSRC_AUTO -12346

class TISCH_SHARED ImageSource {

	public:

		         ImageSource() { }
		virtual ~ImageSource() { }

		virtual int  acquire() = 0;
		virtual void release() = 0;

		virtual void start() = 0;
		virtual void stop () = 0;
		
		virtual void getImage( IntensityImage& target ) const = 0;
		virtual void getImage( RGBImage&       target ) const = 0;

		virtual void setGain( int gain ) = 0;
		virtual void setExposure( int exp ) = 0;
		virtual void setShutter( int speed ) = 0;
		virtual void setBrightness( int bright ) = 0;

		virtual void setFPS( int fps ) = 0;

		virtual void printInfo( int feature = 0 ) = 0;

	protected:

		int width, height, fps;

};

#endif // _IMAGESOURCE_H_

