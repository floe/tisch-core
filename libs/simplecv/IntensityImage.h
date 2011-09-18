/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _INTENSITYIMAGE_H_
#define _INTENSITYIMAGE_H_

#include "Image.h"
#include "Vector.h"
#include <iostream>

class TISCH_SHARED IntensityImage: public Image {

	friend std::ostream& operator<<( std::ostream& s, const IntensityImage& i );
	
	friend class RGBImage;
	friend class YUYVImage;
	friend class YUV420Image;
	friend class YUV420SPImage;
	friend class DCImageSource;

	public:

		IntensityImage( int w, int h, key_t key = 0, unsigned long long int flags = 0 ): Image( w, h, 1, key, flags ) { }
		IntensityImage( const IntensityImage& img ): Image( img ) { }
		IntensityImage( const char* path );

		IntensityImage( const IntensityImage& img, int downsample );
		void sobel( IntensityImage& target );
		void sobel();
		
		void adaptive_threshold( int radius, int bias, IntensityImage& target ) const;

		int  threshold( unsigned char value, IntensityImage& target , unsigned char minvalue = 255 ) const;
		int  threshold( unsigned char value );

		void invert( IntensityImage& target ) const;
		void invert( );

		int histogram( int hg[] ) const;
		int intensity() const;

		long long int integrate( Point start, Vector& centroid, Vector& axis1, Vector& axis2, unsigned char oldcol = 255, unsigned char newcol = 0, std::vector<Point>* border = NULL );

		void undistort( Vector scale, Vector delta, double coeff[5], IntensityImage& target ) const;

		void despeckle( IntensityImage& target, unsigned char threshold = 8 ) const;
		void lowpass( IntensityImage& target, unsigned char range = 1, unsigned char mode = 0 ) const;
		void bandpass( IntensityImage& target, int outer = 16, int inner = 8 ) const;
		void houghLine( IntensityImage& target ) const;
		void areamask( IntensityImage& target, std::vector<int> edgepoints) const;

		void cross( int x, int y, int size, unsigned char color = 255 );
		void box( int x1, int y1, int x2, int y2, unsigned char color = 255 );

		inline int pixelOffset( int x , int y, int channel = 0 ) const { return (y*width)+x; }
		inline unsigned char getPixel( int x, int y, int channel = 0 ) const { return data[(y*width)+x]; }
		inline void setPixel( int x, int y, unsigned char value, int channel = 0 ) { data[(y*width)+x] = value; }

		/*int pixelOffset( int x, int y, int channel = 0 ) const;
		unsigned char getPixel( int x, int y, int channel = 0 ) const;
		void setPixel( int x, int y, unsigned char value, int channel = 0 );*/

		IntensityImage& operator-=( const IntensityImage& arg );
		void subtract( const IntensityImage& i1, const IntensityImage& i2 );

	private:

		struct Moments {
			long long int m00;
			long long int m10;
			long long int m01;
			long long int m11;
			long long int m20;
			long long int m02;
			unsigned char ov;
			unsigned char nv;
		};

		void scanSpan( int add, int dir, int x1, int x2, int y, Moments* m );

		void sobel( unsigned char* target );
		void gradient( char* xgrad, char* ygrad );

};

std::ostream& operator<<( std::ostream& s, const IntensityImage& i );

#endif // _INTENSITYIMAGE_H_

