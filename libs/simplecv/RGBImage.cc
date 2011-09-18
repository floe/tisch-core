/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <Calibration.h>

#include "RGBImage.h"

#include <fstream>
#include <string>

int RGBImage::pixelOffset(int x, int y, int channel) const { return (((y*width)+x)*bpp)+channel; }
unsigned char RGBImage::getPixel(int x, int y, int channel) const { return data[(((y*width)+x)*bpp)+channel]; }
void RGBImage::setPixel(int x, int y, unsigned char value, int channel) { data[(((y*width)+x)*bpp)+channel] = value; }

void RGBImage::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b ) {
	int offset = pixelOffset( x, y, 0 );
	data[offset+0] = r;
	data[offset+1] = g;
	data[offset+2] = b;
}


void RGBImage::getChannel(int channel, IntensityImage& target) const {
	for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) {
		target.setPixel(x,y,getPixel(x,y,channel));
	}
}

RGBImage::RGBImage( const char* path ) {

	int fwidth,fheight,fbpp;
	std::string magic;

	// open file with whitespace skipping
	std::ifstream myfile( path, std::ios::in );
	myfile >> std::skipws;

	// parse the header
	myfile >> magic;
	myfile >> fwidth;
	myfile >> fheight;
	myfile >> fbpp;

	if ((magic != "P6") || (fbpp > 255) || (fbpp < 1)) 
		throw std::runtime_error( std::string("RGBImage: ") + std::string(path) + std::string(": no valid PPM file") );

	// init the base class
	init( fwidth, fheight, 3, 0, 0 );

	// skip one byte, read the rest
	myfile.ignore( 1 );
	myfile.read( (char*)data, size );
	myfile.close( );
}


void RGBImage::getIntensity(IntensityImage& target) const {
	int offset;
	for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) {
		offset = pixelOffset(x,y,0);
		target.setPixel(x,y,(unsigned char)((data[offset+R] + data[offset+G] + data[offset+B]) / 3));
	}
}

void RGBImage::combine(const IntensityImage& red, const IntensityImage& green, const IntensityImage& blue) {
	int chanoffset,rgboffset;
	for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) {
		chanoffset = red.pixelOffset(x,y);
		rgboffset  = pixelOffset(x,y,0);
		data[rgboffset+R] =   red.data[chanoffset];
		data[rgboffset+G] = green.data[chanoffset];
		data[rgboffset+B] =  blue.data[chanoffset];
	}
}

std::ostream& operator<<( std::ostream& s, const RGBImage& i ) {
	s << "P6 " << i.width << " " << i.height << " 255 ";
	s.write( (char*)i.data, i.size );
	return s;	
}

void RGBImage::undistort( Vector scale, Vector delta, double coeff[5], RGBImage& target ) const {

	Vector temp;
	target.clear();

	for (int u = 0; u < width; u++) for (int v = 0; v < height; v++) {

		temp = Vector( u, v, 0 );
		::undistort( temp, scale, delta, coeff );

		if ((temp.x < 0) || (temp.x >=  width)) continue;
		if ((temp.y < 0) || (temp.y >= height)) continue;

		target.setPixel( u, v, getPixel( (int)temp.x, (int)temp.y, R ), R );
		target.setPixel( u, v, getPixel( (int)temp.x, (int)temp.y, G ), G );
		target.setPixel( u, v, getPixel( (int)temp.x, (int)temp.y, B ), B );
	}
}

