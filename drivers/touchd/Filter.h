/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _FILTER_H_
#define _FILTER_H_

#include <tinyxml.h>
#include <algorithm>

#include <typeinfo>
#include <fstream>

#include "GLUTWindow.h"
#include "ShortImage.h"
#include "IntensityImage.h"
#include "RGBImage.h"

#include "Option.h"


#define FILTER_TYPE_NONE   0x00
#define FILTER_TYPE_BASIC  0x01
#define FILTER_TYPE_SHORT  0x02
#define FILTER_TYPE_GREY   0x03
#define FILTER_TYPE_RGB    0x04
#define FILTER_TYPE_ALL    0x07


class Filter {

	public:

		static const int MAX_VALUE = 65535;

		Filter( TiXmlElement* _config = 0, Filter* _input = 0, int _type = FILTER_TYPE_NONE ):
			shmid(0), input(_input), result(0.0), type(_type), config(_config), image(NULL), shortimage(NULL), rgbimage(NULL)
		{
			if (config) config->QueryIntAttribute("ShmID",&shmid);
			// init switching variable for Configurator options
			toggle = 0;

			if (!input) return;

			image = input->getImage();
			if ((type & FILTER_TYPE_BASIC) && image) {
				int w = image->getWidth();
				int h = image->getHeight();
				image = new IntensityImage( w, h, shmid, 1 );
			}

			shortimage = input->getShortImage();
			if ((type & FILTER_TYPE_SHORT) && shortimage) {
				int w = shortimage->getWidth();
				int h = shortimage->getHeight();
				shortimage = new ShortImage( w, h, shmid?shmid+1:0, 1 );
			}

			rgbimage = input->getRGBImage();
			if ((type & FILTER_TYPE_RGB) && rgbimage) {
				int w = rgbimage->getWidth();
				int h = rgbimage->getHeight();
				rgbimage = new RGBImage( w, h, shmid?shmid+2:0, 1 );
			}
		}

		virtual ~Filter() {
			delete image;
			delete shortimage;
			delete rgbimage;
		}

		virtual int process() = 0;
		virtual void reset(int initialReset) { }
		virtual void processMouseButton(int button, int state, int x, int y) { }

		virtual void draw( GLUTWindow* win, int show_image = FILTER_TYPE_BASIC ) {

			switch (show_image) {
				case FILTER_TYPE_BASIC: 
				case FILTER_TYPE_SHORT: if (shortimage) { win->show( *shortimage, 0, 0 ); break; }
				                        if (image)      { win->show( *image,      0, 0 ); break; }
				case FILTER_TYPE_RGB:   if (rgbimage)   { win->show( *rgbimage,   0, 0 ); break; }
			}

			std::string filter = typeid( *this ).name();
			const char* name = filter.c_str();
			while (name && (*name >= '0') && (*name <= '9')) name++;

			glColor4f( 1.0, 0.0, 0.0, 1.0 );
			win->print( std::string("showing filter: ") + name, 10, 10 );
		}

		virtual void link( Filter* _link ) { }

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
		virtual TiXmlElement* getXMLRepresentation() {return new TiXmlElement( "something_went_wrong" );};
		Filter* getParent() {return input;};

	protected:

		int shmid;
		Filter* input;
		double result;
		int type;

		TiXmlElement* config;
		IntensityImage* image;
		ShortImage* shortimage;
		RGBImage* rgbimage;

		// Configurator
		int toggle; // initialized in basic Filter constructor
		int countOfOptions; // Initialization required in each subfilter class !
		int resetOnInit;
};

#endif // _FILTER_H_
