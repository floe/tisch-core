/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "YUV420SPImage.h"
#include "ColorLUT.h"
#include <string.h>
#include <iostream>

inline int YUV420SPImage::pixelOffset(int x, int y, int channel) const {
	switch (channel) {
		case Y: return (y*width)+x;
		case U: return ((y>>1)*width)+2*(x>>1)+(width*height)+1; 
		case V: return ((y>>1)*width)+2*(x>>1)+(width*height); 
	}
	return 0;
}

inline unsigned char YUV420SPImage::getPixel(int x, int y, int channel) const { return data[pixelOffset(x,y,channel)]; }
inline void YUV420SPImage::setPixel(int x, int y, unsigned char value, int channel) { data[pixelOffset(x,y,channel)] = value; }

void YUV420SPImage::getImage( RGBImage& target ) {

	register unsigned char Y1, Cb, Cr;
	register unsigned char r, g, b;

	for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) {

		Y1 = getPixel(x,y,Y);
		Cb = getPixel(x,y,U);
		Cr = getPixel(x,y,V);

		r = R_FROMYV(Y1,Cr);
		g = G_FROMYUV(Y1,Cr,Cb);
		b = B_FROMYU(Y1,Cb);

		target.setPixel(x,y,r,R);
		target.setPixel(x,y,g,G);
		target.setPixel(x,y,b,B);
	}
}

void YUV420SPImage::getImage( IntensityImage& target ) {
	memcpy( target.data, data, width*height );
}

