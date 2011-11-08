/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _SHORTIMAGE_H_
#define _SHORTIMAGE_H_

#include "IntensityImage.h"

class TISCH_SHARED ShortImage: public Image {

	friend class IntensityImage;

	public:

		ShortImage( int w, int h );

		ShortImage( const ShortImage&     img );
		ShortImage( const IntensityImage& img );

		ShortImage& operator= ( const ShortImage&     img );
		ShortImage& operator= ( const IntensityImage& img );

		void update( const IntensityImage& img, const IntensityImage& mask );
		void update( const ShortImage& img, const ShortImage& mask );
		void subtract( const IntensityImage& source, IntensityImage& target, int invert );
		void subtract( const ShortImage& source, ShortImage& target, int invert );
		void convert( IntensityImage& img );

		int intensity() const;
		void undistort( Vector scale, Vector delta, double coeff[5], ShortImage& target ) const;

		unsigned short getPixel(int x, int y) const; 
		void setPixel(int x, int y, unsigned short value);

		 unsigned short* getSData() const { return sdata; }

		 int  threshold( unsigned short value, ShortImage& target , unsigned short minvalue = 65535 ) const;
		 void despeckle( ShortImage& target, unsigned char threshold = 8 ) const;
		 void lowpass( ShortImage& target, unsigned char range = 1, unsigned char mode = 0 ) const;
		 void areamask( ShortImage& target, std::vector<int> edgepoints) const;

	private:

		unsigned short* sdata;

};

#endif // _SHORTIMAGE_H_

