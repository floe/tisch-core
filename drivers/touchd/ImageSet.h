/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _IMAGESET_H_
#define _IMAGESET_H_

#include <string> 

#include "GLUTWindow.h"
#include "ShortImage.h"
#include "Settings.h"

class ImageSet {

	public:

		ImageSet( Settings* _settings );
		~ImageSet();

		// retrieve raw/final image
		IntensityImage* getRaw();
		IntensityImage* getFinal();

		// swap the raw image with another ImageSet
		void swap( ImageSet* other );

		// subtract + update background, apply threshold and denoise
		void process();

		// paint the selected image into a window and return its name
		std::string draw( GLUTWindow* win, int num );

		// reset the background
		void update();

		// store & return average intensity
		int analyze();

	private:

		IntensityImage* raw;
		IntensityImage* mask;
		IntensityImage* binary;
		ShortImage*     bkgnd;
		IntensityImage* subtr;
		IntensityImage* final;

		Settings* settings;

		int bg_intensity;
		int img_intensity;
};

#endif // _IMAGESET_H_

