/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _FILTER_H_
#define _FILTER_H_

#include <tinyxml.h>

#include "ImageSource.h"
#include "ShortImage.h"
#include "IntensityImage.h"


class Filter {

	public:

		Filter( TiXmlElement* _config = 0, Filter* _input = 0 ): input(_input), result(0.0), config(_config), image(NULL) { }
		virtual ~Filter() { delete image; }

		virtual void process() = 0;
		IntensityImage* getImage() { return image; }
		double getResult() { return result; }

	protected:

		Filter* input;
		double result;
		TiXmlElement* config;
		IntensityImage* image;
};


class SourceFilter: public Filter {

	public:
		
		SourceFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual ~SourceFilter();

		virtual void process();

	protected:

		ImageSource* imgsrc;
};

class BGSubFilter: public Filter {

	public:

		BGSubFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual ~BGSubFilter();

		virtual void process();

	protected:

		ShortImage* background;
		int invert;
};

class ThreshFilter: public Filter {

	public:

		ThreshFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual ~ThreshFilter();

		virtual void process();

	protected:

		int threshold;
};

class DespeckleFilter: public Filter {

	public:

		DespeckleFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual ~DespeckleFilter();

		virtual void process();
	
	protected:

		int noisecount;
};

#endif // _FILTER_H_

