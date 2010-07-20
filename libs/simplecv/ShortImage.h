/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
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
		void subtract( const IntensityImage& source, IntensityImage& target, int invert );
		void convert( IntensityImage& img );

		int intensity() const;

	private:

		unsigned short* sdata;

};

#endif // _SHORTIMAGE_H_

