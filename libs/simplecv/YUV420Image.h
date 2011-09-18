/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _YUV420IMAGE_H_
#define _YUV420IMAGE_H_

#include "RGBImage.h"

#define Y 0
#define U 1
#define V 2

class YUV420Image: public Image {

	friend class V4LImageSource;

	public:

		YUV420Image(int w, int h): Image(w,h,2) { }

		void getImage( IntensityImage& target );
		void getImage( RGBImage&       target );

		int pixelOffset(int x, int y, int channel) const;
		unsigned char getPixel(int x, int y, int channel) const;
		void setPixel(int x, int y, unsigned char value, int channel);

};

#endif // _YUV420IMAGE_H_

