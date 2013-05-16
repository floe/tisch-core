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
	data[offset + TR]	= r;
	data[offset + TG]	= g;
	data[offset + TB]	= b;
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
	for (int offset = 0; offset < size; offset+=3) {
		target.data[offset/3] = (data[offset + TR] + data[offset + TG] + data[offset + TB]) / 3;
	}
}

void RGBImage::getHSV( IntensityImage& hue, IntensityImage& sat, IntensityImage& val ) const {
	unsigned char r,g,b,max,min,h,s,v,c;
	int target_offs = 0;
	for (int offset = 0; offset < size; offset+=3) {

		r = data[offset + TR];
		g = data[offset + TG];
		b = data[offset + TB];

		max = (r > g ? r : g); max = (max > b ? max : b);
		min = (r < g ? r : g); min = (min < b ? min : b);

		c = max - min;
		v = max;

		if (c == 0) {
			h = 0;
			s = 0;
		} else {
			s = 255*(int)c/v;
			if (r == max) {
				h = 0 + 43*(g - b)/c;
			} else if (g == max) {
				h = 85 + 43*(b - r)/c;
			} else { // b == max
				h = 171 + 43*(r - g)/c;
			}
		}

		hue.data[target_offs] = h;
		sat.data[target_offs] = s;
		val.data[target_offs] = v;
		target_offs++;
	}
}

void RGBImage::combine(const IntensityImage& hue, const IntensityImage& sat, const IntensityImage& val) {

	/* original (unused) code for combining RGB
	int chanoffset,rgboffset;
	for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) {
		chanoffset = red.pixelOffset(x,y);
		rgboffset  = pixelOffset(x,y,0);
		data[rgboffset + TR]	=   red.data[chanoffset];
		data[rgboffset + TG]	= green.data[chanoffset];
		data[rgboffset + TB]	=  blue.data[chanoffset];
	}*/

	unsigned char r,g,b,area,rest,p,q,t,h,s,v;
	int target_offset = 0;

	for (int offset = 0; offset < count; offset++) {

		h = hue.data[offset];
		s = sat.data[offset];
		v = val.data[offset];

		if (s == 0) {
			r = v;
			g = v;
			b = v;
		} else {
			area = h / 43;
			rest = h % 43; //?
			p = ((int)v * ( 255 - s )) >> 8;
			q = ((int)v * ( 255 - ((s * rest) >> 8))) >> 8;
			t = ((int)v * ( 255 - ((s * (255-rest)) >> 8))) >> 8;
			switch (area) {
				case 0:  r = v; g = t; b = p; break;
				case 1:  r = q; g = v; b = p; break;
				case 2:  r = p; g = v; b = t; break;
				case 3:  r = p; g = q; b = v; break;
				case 4:  r = t; g = p; b = v; break;
				default: r = v; g = p; b = q; break;
			}
		}

		data[target_offset+TR] = r;
		data[target_offset+TG] = g;
		data[target_offset+TB] = b;
		target_offset+=3;
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

		target.setPixel( u, v, getPixel( (int)temp.x, (int)temp.y, TR ), TR );
		target.setPixel( u, v, getPixel( (int)temp.x, (int)temp.y, TG ), TG );
		target.setPixel( u, v, getPixel( (int)temp.x, (int)temp.y, TB ), TB );
	}
}

