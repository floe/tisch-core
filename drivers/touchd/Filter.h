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

		Filter( TiXmlElement* _config = 0, Filter* _input = 0 ):
			shmid(0), input(_input), result(0.0), config(_config), image(NULL), shortimage(NULL)
		{ 
			if (config) config->QueryIntAttribute("ShmID",&shmid);
			// init switching variable for Configurator options
			toggle = 0;
			MAX_VALUE = 65535;
			useIntensityImage = input->getUseIntensityImage();
		}

		virtual ~Filter() { delete image; delete shortimage; }

		void checkImage() {
			if (!image) {
				IntensityImage* inputimg = input->getImage();
				int w = inputimg->getWidth();
				int h = inputimg->getHeight();
				image = new IntensityImage( w, h, shmid, 1 );
			}
			if (/*TODO: do we have to add this? !useIntensityImage &&*/ !shortimage) {
				ShortImage* inputimg = input->getShortImage();
				int w = inputimg->getWidth();
				int h = inputimg->getHeight();
				shortimage = new ShortImage( w, h );
			}
		}

		virtual int process() = 0;
		virtual void reset() { }

		// TODO: print filter information
		virtual void draw( GLUTWindow* win ) { if(useIntensityImage) win->show( *image, 0, 0 ); else win->show( *shortimage, 0, 0 ); }
		virtual void link( Filter* _link   ) { }

		virtual IntensityImage* getImage() { return image; }
		virtual ShortImage* getShortImage() { return shortimage; }
		virtual double getResult() { return result; }

		// Configurator functions
		void nextOption() { toggle = (countOfOptions > 0) ? ((toggle + 1) % countOfOptions) : toggle; }
		int getCurrentOption() { return toggle; }
		const int getOptionCount() { return countOfOptions; }
		virtual const char* getOptionName(int option) { return ""; };
		virtual double getOptionValue(int option) { return -1;};
		virtual void modifyOptionValue(double delta, bool overwrite) { };
		bool getUseIntensityImage() { return useIntensityImage; };

	protected:

		int shmid;
		Filter* input;
		double result;
		TiXmlElement* config;
		IntensityImage* image;
		ShortImage* shortimage;
		bool useIntensityImage;
		// Configurator
		int toggle; // initialized in basic Filter constructor
		int MAX_VALUE; // initialized in basic Filter constructor
		int countOfOptions; // Initialization required in each subfilter class !
};


// TODO add highpass filter from ITS paper


class BGSubFilter: public Filter {
	public:
		BGSubFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual ~BGSubFilter();
		virtual int process();
		virtual void reset();
		virtual void link( Filter* _mask );
		// Configurator
		virtual const char* getOptionName(int option);
		virtual double getOptionValue(int option);
		virtual void modifyOptionValue(double delta, bool overwrite);
	protected:
		ShortImage* background;
		Filter* mask;
		int invert, adaptive;

};

class FlipFilter: public Filter {
	public:
		FlipFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual int process();
		// Configurator
		virtual const char* getOptionName(int option);
		virtual double getOptionValue(int option);
		virtual void modifyOptionValue(double delta, bool overwrite);
	protected:
		// Options
		int hflip;
		int vflip;
};

class ThreshFilter: public Filter {
	public:
		ThreshFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual int process();
		// Configurator
		virtual const char* getOptionName(int option);
		virtual double getOptionValue(int option);
		virtual void modifyOptionValue(double delta, bool overwrite);
	protected:
		// Options
		int threshold_min;
		int threshold_max;
};

class SpeckleFilter: public Filter {
	public:
		SpeckleFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual int process();
		// Configurator
		virtual const char* getOptionName(int option);
		virtual double getOptionValue(int option);
		virtual void modifyOptionValue(double delta, bool overwrite);
	protected:
		int noiselevel;
};

class LowpassFilter: public Filter {
	public:
		LowpassFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual int process();
		// Configurator
		virtual const char* getOptionName(int option);
		virtual double getOptionValue(int option);
		virtual void modifyOptionValue(double delta, bool overwrite);
	protected:
		int mode, range;
};

class SplitFilter: public Filter {
	public:
		SplitFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual int process();
		virtual void reset();
		virtual IntensityImage* getImage();
	protected:
		IntensityImage* image2;
		int incount, outcount;
};

#endif // _FILTER_H_

