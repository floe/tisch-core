/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "YUYVImage.h"
#include "ColorLUT.h"

#include <string.h>
#include <iostream>


inline int YUYVImage::pixelOffset(int x, int y, int channel) const {
	switch (channel) {
		case Y: return ((y*width)+x)*2;
		case U: return (y*width*2)+((x/2)*4)+1;
		case V: return (y*width*2)+((x/2)*4)+3;
	}
	return 0;
}

inline unsigned char YUYVImage::getPixel(int x, int y, int channel) const { return data[pixelOffset(x,y,channel)]; }
inline void YUYVImage::setPixel(int x, int y, unsigned char value, int channel) { data[pixelOffset(x,y,channel)] = value; }


void YUYVImage::getImage( RGBImage& target ) {

	register unsigned char y1, y2, u, v, r, g, b;
	register int i, max = width*height;
	unsigned char* otmp = target.data;

	for (i = 0; i < max; i+=2) {

		y1 = data[i*2];
		u =  data[i*2+1];
		y2 = data[i*2+2];
		v =  data[i*2+3];

		r = R_FROMYV(y1,v);
		g = G_FROMYUV(y1,u,v);
		b = B_FROMYU(y1,u);

		otmp[i*3] = r;
		otmp[i*3+1] = g;
		otmp[i*3+2] = b;

		r = R_FROMYV(y2,v);
		g = G_FROMYUV(y2,u,v);
		b = B_FROMYU(y2,u);

		otmp[i*3+3] = r;
		otmp[i*3+4] = g;
		otmp[i*3+5] = b;
	}
}

void YUYVImage::getImage( IntensityImage& target ) {
	memcpy( target.data, data, width*height );
	//for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) 
		//target.setPixel( x, y, getPixel( x, y, Y ) );
	for (int i = 0; i < width*height; i++) 
		target.data[i] = data[i*2];
}

