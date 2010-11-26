/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _FILTER_H_
#define _FILTER_H_

#include <tinyxml.h>

#include "GLUTWindow.h"
#include "ShortImage.h"
#include "IntensityImage.h"


class Filter {

	public:

		Filter( TiXmlElement* _config = 0, Filter* _input = 0 ): input(_input), result(0.0), config(_config), image(NULL) { }
		virtual ~Filter() { delete image; }

		virtual void process() = 0;
		virtual void reset() { }

		virtual void draw( GLUTWindow* win ) { win->show( *image, 0, 0 ); }
		virtual void link( Filter* _link   ) { }

		void checkImage() {
			if (!image) {
				int w = input->getImage()->getWidth();
				int h = input->getImage()->getHeight();
				image = new IntensityImage( w, h );
			}
		}

		inline IntensityImage* getImage() const { return image; }
		inline double getResult()  const { return result; }

	protected:

		Filter* input;
		double result;
		TiXmlElement* config;
		IntensityImage* image;
};


class BGSubFilter: public Filter {
	public:
		BGSubFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual ~BGSubFilter();
		virtual void process();
		virtual void reset();
		virtual void link( Filter* _mask );
	protected:
		ShortImage* background;
		Filter* mask;
		int invert;
};

class ThreshFilter: public Filter {
	public:
		ThreshFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual void process();
	protected:
		int threshold;
};

class SpeckleFilter: public Filter {
	public:
		SpeckleFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual void process();
	protected:
		int noiselevel;
};

#endif // _FILTER_H_

