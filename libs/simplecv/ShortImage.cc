/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "ShortImage.h"
#include "mmx.h"

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

