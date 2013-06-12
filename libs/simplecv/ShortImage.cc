/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <Calibration.h>

#include "ShortImage.h"
#include "mmx.h"

unsigned short ShortImage::getPixel(int x, int y) const { return ((unsigned short*)data)[(y*width)+x]; }
void ShortImage::setPixel(int x, int y, unsigned short value) { ((unsigned short*)data)[(y*width)+x] = value; }

ShortImage::ShortImage( int w, int h, key_t key, unsigned long long int flags ): Image( w, h, sizeof(unsigned short), key, flags ) { sdata = (unsigned short*)data; }
ShortImage::ShortImage( const ShortImage& img ): Image( img ) { sdata = (unsigned short*)data; }

ShortImage::ShortImage( const IntensityImage& img ):
	Image( img.getWidth(), img.getHeight(), sizeof(unsigned short) )
{
	sdata = (unsigned short*)data;
	unsigned char* imgdata = img.getData();
	for (int i = 0; i < count; i++) sdata[i] = imgdata[i] << 8;
}

ShortImage::ShortImage( const char* filename ) {
	Image::load( filename, "P5", 2 );
}

void ShortImage::save( const char* path ) {
	Image::save( path, "P5" );
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

void ShortImage::update( const ShortImage& img, const ShortImage& mask ) {
//	#ifdef NOMMX
		unsigned short* imgdata = img.getSData();
		unsigned short* maskdata = mask.getSData();
		for (int i = 0; i < count; i++)
			if (maskdata[i] == 0) sdata[i] = sdata[i] - (sdata[i] >> 1) + (imgdata[i] >> 1);
//	#else
//		mmxupdate( img.getData(), mask.getData(), sdata, count );
//	#endif
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

void ShortImage::subtract( const ShortImage& source, ShortImage& target, int invert ) {
//	#ifdef NOMMX
		int tmp;
		unsigned short* sourcedata = source.getSData();
		unsigned short* targetdata = target.getSData();
		if (invert) 
			for (int i = 0; i < count; i++) {
				tmp = sdata[i] - sourcedata[i];
				targetdata[i] = (unsigned short)((tmp > 0) ? ((tmp < 65535) ? tmp : 65535 ) : 0);
			}
		else
			for (int i = 0; i < count; i++) {
				tmp = (int)sourcedata[i] - sdata[i];
				targetdata[i] = (unsigned short)((tmp > 0) ? ((tmp < 65535) ? tmp : 65535 ) : 0);
			}
//	#else
//		if (invert)
//			mmxsubtract( sdata, source.getData(), target.getData(), count );
//		else
//			mmxsubtract( source.getData(), sdata, target.getData(), count );
//	#endif
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

int ShortImage::threshold( unsigned short value, ShortImage& target , unsigned short minvalue ) const {
//	#ifndef NOMMX
	//	mmxthreshold( data, target.data, size, value, minvalue );
//		return 0;
//	#else
		unsigned short tmp;
		int hits = 0;
		for (int i = 0; i < count; i++) {
			tmp = ((sdata[i] > value) && (sdata[i] <= minvalue)) ? 65535 : 0;
			if (tmp) hits++;
			target.sdata[i] = tmp;
		}
		return hits;
//	#endif
}

void ShortImage::despeckle( ShortImage& target, unsigned char threshold ) const {
/*	#ifndef NOMMX
		memset( target.data, 0, width );
		memset( target.data+(width*(height-1)), 0, width );
		for (int j = 0; j < width-7; j+=6) mmxdespeckle( data+j, target.data+j, height, width, threshold );
		mmxdespeckle( data+width-7, target.data+width-7, height, width, threshold );
	#else*/
		int neighbor[] = { -width-1, -width, -width+1, -1, 0, +1, width-1, width, width+1 };
		int sum;
		target.clear();
		for (int i = width+1; i < count-width-1; i++) {
			sum = 0;
			for (int j = 0; j < 9; j++) if (sdata[i+neighbor[j]]) sum++;
			target.sdata[i] = (sum >= threshold) ? 65535 : 0;
		}
//	#endif
}

void ShortImage::lowpass( ShortImage& target, unsigned char range, unsigned char mode ) const {
	int sum;
	target.clear();
	if(mode == 0) //horizontal
	{
		for (int i = range; i < count-range; i++) 
		{
			sum = 0;
			for (int j = -range; j <= range; j++) if (sdata[i+j]) sum++;
			target.sdata[i] = (sum == 2*range + 1) ? 65535 : 0;
		}
	}
	else if (mode == 1) //vertical
	{
		for (int i = range*width; i < count-range*width; i++) 
		{
			sum = 0;
			for (int j = -range; j <= range; j++) if (sdata[i+j*width]) sum++;
			target.sdata[i] = (sum == 2*range + 1) ? 65535 : 0;
		}
	}
	else if (mode == 2) //horizontal + vertical
	{
		for (int i = range*width + range; i < count-range*width - range; i++) 
		{
			sum = 0;
			for (int j = -range; j <= range; j++) {
				if (sdata[i+j]) sum++; 
				if (sdata[i+j*width]) sum++;
			}
			target.sdata[i] = (sum == 4*range + 2) ? 65535 : 0;
		}
	}
}

void ShortImage::areamask( ShortImage& target, std::vector<int> edgepoints) const
{
	if( edgepoints.empty() )
		memcpy(target.sdata,sdata,width*height*sizeof(unsigned short));
	else
		memset(target.sdata, 0, width*height*sizeof(unsigned short));
		for(std::vector<int>::iterator it = edgepoints.begin(); it != edgepoints.end(); it = it + 2)
			memcpy((target.sdata +  (*it)), (sdata + (*it)), (*(it+1) - *it)*2); 
}

