/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <Calibration.h>

#include "ShortImage.h"
#include "mmx.h"

unsigned short ShortImage::getPixel(int x, int y) const { return ((unsigned short*)data)[(y*width)+x]; }
void ShortImage::setPixel(int x, int y, unsigned short value) { ((unsigned short*)data)[(y*width)+x] = value; }

ShortImage::ShortImage( int w, int h ): Image( w, h, sizeof(unsigned short) ) { sdata = (unsigned short*)data; }
ShortImage::ShortImage( const ShortImage& img ): Image( img ) { sdata = (unsigned short*)data; }

ShortImage::ShortImage( const IntensityImage& img ):
	Image( img.getWidth(), img.getHeight(), sizeof(unsigned short) )
{
	sdata = (unsigned short*)data;
	unsigned char* imgdata = img.getData();
	for (int i = 0; i < count; i++) sdata[i] = imgdata[i] << 8;
}


ShortImage& ShortImage::operator= ( const IntensityImage& img ) {
	//if (this == &img) return *this;
	if (count != img.getCount()) throw std::runtime_error( "Image: raw size mismatch in assignment" );
	unsigned char* imgdata = img.getData();
	for (int i = 0; i < count; i++) sdata[i] = imgdata[i] << 8;
	return *this;
}

ShortImage& ShortImage::operator= ( const ShortImage& img ) {
	if (this == &img) return *this;
	if (count != img.getCount()) throw std::runtime_error( "Image: raw size mismatch in assignment" );
	memcpy( data, img.getData(), size );
	return *this;
}


void ShortImage::update( const IntensityImage& img, const IntensityImage& mask ) {
	#ifdef NOMMX
		unsigned char* imgdata = img.getData();
		unsigned char* maskdata = mask.getData();
		for (int i = 0; i < count; i++)
			if (maskdata[i] == 0) sdata[i] = sdata[i] - (sdata[i] >> 9) + (imgdata[i] >> 1);
	#else
		mmxupdate( img.getData(), mask.getData(), sdata, count );
	#endif
}


void ShortImage::subtract( const IntensityImage& source, IntensityImage& target, int invert ) {
	#ifdef NOMMX
		int tmp;
		unsigned char* sourcedata = source.getData();
		unsigned char* targetdata = target.getData();
		if (invert) 
			for (int i = 0; i < count; i++) {
				tmp = (sdata[i] >> 8) - sourcedata[i];
				targetdata[i] = (unsigned char)((tmp > 0) ? ((tmp < 255) ? tmp : 255 ) : 0);
			}
		else
			for (int i = 0; i < count; i++) {
				tmp = (int)sourcedata[i] - (sdata[i] >> 8);
				targetdata[i] = (unsigned char)((tmp > 0) ? ((tmp < 255) ? tmp : 255 ) : 0);
			}
	#else
		if (invert)
			mmxsubtract( sdata, source.getData(), target.getData(), count );
		else
			mmxsubtract( source.getData(), sdata, target.getData(), count );
	#endif
}


void ShortImage::convert( IntensityImage& img ) {
	if (count != img.getCount()) throw std::runtime_error( "Image: raw size mismatch in assignment" );
	unsigned char* imgdata = img.getData();
	for (int i = 0; i < count; i++) imgdata[i] = (unsigned char)(sdata[i] >> 8);
}


int ShortImage::intensity() const {
	#ifdef NOMMX
		long long int tmp = 0;
		for (int i = 0; i < count; i++) tmp += data[i];
		return (int)(tmp/(long long int)count);
	#else
		return mmxintensity( sdata, count );
	#endif
}

void ShortImage::undistort( Vector scale, Vector delta, double coeff[5], ShortImage& target ) const {

	Vector temp;
	target.clear();

	for (int u = 0; u < width; u++) for (int v = 0; v < height; v++) {

		temp = Vector( u, v, 0 );
		::undistort( temp, scale, delta, coeff );

		if ((temp.x < 0) || (temp.x >=  width)) continue;
		if ((temp.y < 0) || (temp.y >= height)) continue;

		target.setPixel( u, v, getPixel( (int)temp.x, (int)temp.y ) );
	}
}

