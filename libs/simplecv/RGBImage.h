/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _RGBIMAGE_H_
#define _RGBIMAGE_H_

#include "IntensityImage.h"

#define R 0
#define G 1
#define B 2

class TISCH_SHARED RGBImage: public Image {

	friend std::ostream& operator<<( std::ostream& s, const RGBImage& i );

	friend class YUYVImage;
	
	public:

		RGBImage(int w, int h): Image(w,h,3) { }
		RGBImage( const char* path );

		void getChannel(int channel, IntensityImage& target) const;
		void getIntensity(IntensityImage& target) const;

		void combine(const IntensityImage& red, const IntensityImage& green, const IntensityImage& blue);

		int pixelOffset(int x, int y, int channel) const;
		unsigned char getPixel(int x, int y, int channel) const;
		void setPixel(int x, int y, unsigned char value, int channel);
		void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b );

		void undistort( Vector scale, Vector delta, double coeff[5], RGBImage& target ) const; 

};

std::ostream& operator<<( std::ostream& s, const RGBImage& i );

#endif // _RGBIMAGE_H_

