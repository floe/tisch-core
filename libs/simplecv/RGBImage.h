/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _RGBIMAGE_H_
#define _RGBIMAGE_H_

#include "IntensityImage.h"

#define TR	0
#define TG	1
#define TB	2

class TISCH_SHARED RGBImage: public Image {

	friend std::ostream& operator<<( std::ostream& s, const RGBImage& i );

	friend class YUYVImage;
	
	public:

		RGBImage( int w, int h, key_t key = 0, unsigned long long int flags = 0 ): Image( w, h, 3, key, flags ) { }
		RGBImage( const char* path );

		void getChannel(int channel, IntensityImage& target) const;
		void getIntensity(IntensityImage& target) const;
		void getHSV( IntensityImage& hue, IntensityImage& sat, IntensityImage& val ) const;

		void combine(const IntensityImage& red, const IntensityImage& green, const IntensityImage& blue);

		int pixelOffset(int x, int y, int channel) const;
		unsigned char getPixel(int x, int y, int channel) const;
		void setPixel(int x, int y, unsigned char value, int channel);
		void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b );

		void undistort( Vector scale, Vector delta, double coeff[5], RGBImage& target ) const; 

};

void TISCH_SHARED rgb2hsv( int r, int g, int b, unsigned char &h, unsigned char &s, unsigned char &v );

std::ostream& operator<<( std::ostream& s, const RGBImage& i );

#endif // _RGBIMAGE_H_

