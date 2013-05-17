/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _FILTER_H_
#define _FILTER_H_

#include <tinyxml.h>
#include <algorithm>

#include <fstream>

#include "GLUTWindow.h"
#include "ShortImage.h"
#include "IntensityImage.h"
#include "RGBImage.h"

class Filter {

	public:

		static const int MAX_VALUE = 65535;

		Filter( TiXmlElement* _config = 0, Filter* _input = 0 ):
			shmid(0), input(_input), result(0.0), config(_config), image(NULL), shortimage(NULL), rgbimage(NULL)
		{ 
			if (config) config->QueryIntAttribute("ShmID",&shmid);
			// init switching variable for Configurator options
			toggle = 0;
			if(input != 0) {
				useIntensityImage = input->getUseIntensityImage();
				displayRGBImage = input->getdisplayRGBImage();
			}
		}

		virtual ~Filter() { delete image; delete shortimage; delete rgbimage; }

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
			if(!rgbimage) {
				RGBImage* inputimg = input->getRGBImage();
				int w = inputimg->getWidth();
				int h = inputimg->getHeight();
				rgbimage = new RGBImage( w, h );
			}
		}

		virtual int process() = 0;
		virtual void reset(int initialReset) { }
		virtual void processMouseButton(int button, int state, int x, int y) { }

		// TODO: print filter information
		virtual void draw( GLUTWindow* win ) {
			if(useIntensityImage)
				win->show( *image, 0, 0 );
			else if( displayRGBImage )
				win->show( *rgbimage, 0, 0 );
			else
				win->show( *shortimage, 0, 0 );
		}
		virtual void link( Filter* _link   ) { }

		virtual IntensityImage* getImage() { return image; }
		virtual ShortImage* getShortImage() { return shortimage; }
		virtual RGBImage* getRGBImage() { return rgbimage; }
		virtual double getResult() { return result; }

		// Configurator functions
		void nextOption() { toggle = (countOfOptions > 0) ? ((toggle + 1) % countOfOptions) : toggle; }
		int getCurrentOption() { return toggle; }
		const int getOptionCount() { return countOfOptions; }
		virtual const char* getOptionName(int option) { return ""; };
		virtual double getOptionValue(int option) { return -1;};
		virtual void modifyOptionValue(double delta, bool overwrite) { };
		int getUseIntensityImage() { return useIntensityImage; };
		virtual TiXmlElement* getXMLRepresentation() {return new TiXmlElement( "something_went_wrong" );};
		Filter* getParent() {return input;};

		int getdisplayRGBImage() { return displayRGBImage; };
		void showRGBImage() { displayRGBImage = (displayRGBImage + 1) % 2; };

	protected:

		int shmid;
		Filter* input;
		double result;
		TiXmlElement* config;
		IntensityImage* image;
		ShortImage* shortimage;
		RGBImage* rgbimage;
		int useIntensityImage;
		// Configurator
		int displayRGBImage;
		int toggle; // initialized in basic Filter constructor
		int countOfOptions; // Initialization required in each subfilter class !
		int resetOnInit;
};

#endif // _FILTER_H_
