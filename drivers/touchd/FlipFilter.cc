/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "FlipFilter.h"

FlipFilter::FlipFilter( TiXmlElement* _config, Filter* _input ):
	Filter( _config, _input, FILTER_TYPE_ALL )
{
	createOption( "HFlip", 0, 0, 1 );
	createOption( "VFlip", 0, 0, 1 );
}

// TODO: should be MMX-accelerated
int FlipFilter::process() {

	int width = 0;
	int height = 0;

	int hflip = options["HFlip"].get();
	int vflip = options["VFlip"].get();

	if(image) 
	{
		unsigned char* inbuf = input->getImage()->getData();
		unsigned char* outbuf = image->getData();

		width  = image->getWidth();
		height = image->getHeight();

		// no flipping
		if (vflip == 0 && hflip == 0) {
			for (int i = 0; i < height * width; i++) {
				outbuf[i] = inbuf[i];
			}
		}

		// horizontal flipping - flipping along vertical axis
		if(vflip == 0 && hflip == 1){
			int inoffset  = 0;
			int outoffset = width - 1; // same line last index
		
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) outbuf[outoffset - x] = inbuf[inoffset + x];
				inoffset  += width;
				outoffset += width;
			}
		}

		// vertical flipping - flipping along horizontal axis
		if(vflip == 1 && hflip == 0){
			int inoffset  = 0;
			int outoffset = (height * width) - width; // beginning of the last line

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) outbuf[outoffset + x] = inbuf[inoffset + x];
				inoffset  += width;
				outoffset -= width;
			}
		}

		// vertical && horizontal flipping
		// reversing the order of the pixel
		if(vflip == 1 && hflip == 1) {
			int inoffset  = 0;
			int outoffset = (height * width) - 1; // very last element of the array

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) outbuf[outoffset - x] = inbuf[inoffset + x];
				inoffset  += width;
				outoffset -= width;
			}
		}
	}
	
	if (shortimage) {
		// Kinect Depth Image

		unsigned short* inbuf  = input->getShortImage()->getSData();
		unsigned short* outbuf = shortimage->getSData();

		width  = shortimage->getWidth();
		height = shortimage->getHeight();

		// no flipping
		if (vflip == 0 && hflip == 0) {
			for (int i = 0; i < height * width; i++) {
				outbuf[i] = inbuf[i];
			}
		}

		// horizontal flipping - flipping along the vertical axis
		if(vflip == 0 && hflip == 1){
			int inoffset  = 0;
			int outoffset = width - 1; // same line last index
		
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) outbuf[outoffset - x] = inbuf[inoffset + x];
				inoffset  += width;
				outoffset += width;
			}
		}

		// vertical flipping - flipping along the horizontal axis
		if(vflip == 1 && hflip == 0){
			int inoffset  = 0;
			int outoffset = (height * width) - width; // beginning of the last line

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) outbuf[outoffset + x] = inbuf[inoffset + x];
				inoffset  += width;
				outoffset -= width;
			}
		}

		// vertical && horizontal flipping
		// reversing the order of the pixel
		if(vflip == 1 && hflip == 1) {
			int inoffset  = 0;
			int outoffset = (height * width) - 1; // very last element of the array

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) outbuf[outoffset - x] = inbuf[inoffset + x];
				inoffset  += width;
				outoffset -= width;
			}
		}
	}

	// Kinect RGB Image

	if (rgbimage) {
	unsigned char* inputIMG  = input->getRGBImage()->getData(); // get pointer form previous filter
	unsigned char* outputIMG = rgbimage->getData(); // prepare for processed filter output
	
	// flip RGB Image
	width = rgbimage->getWidth();
	height = rgbimage->getHeight();
	
	// no flipping
	if (vflip == 0 && hflip == 0) {
		for(int i = 0; i < height * width * 3; i += 3) {
			outputIMG[i]	= inputIMG[i];		// r
			outputIMG[i+1]	= inputIMG[i+1];	// g
			outputIMG[i+2]	= inputIMG[i+2];	// b
		}
	}
	
	// horizontal flipping - flipping along vertical axis
	if(vflip == 0 && hflip == 1) {
		int inoffset  = 0;
		int outoffset = (width - 1) * 3; // same line last index
		
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width * 3; x += 3) {
				outputIMG[outoffset - x]		= inputIMG[inoffset + x]; // r
				outputIMG[outoffset - x + 1]	= inputIMG[inoffset + x +1]; // g
				outputIMG[outoffset - x + 2]	= inputIMG[inoffset + x +2]; // b
			}
			inoffset  += width * 3;
			outoffset += width * 3;
		}
	}

	// vertical flipping - flipping along horizontal axis
	if(vflip == 1 && hflip == 0){
		int inoffset  = 0;
		int outoffset = (height * width * 3) - width * 3; // beginning of the last line

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width * 3; x += 3) {
				outputIMG[outoffset + x]		= inputIMG[inoffset + x]; // r
				outputIMG[outoffset + x + 1]	= inputIMG[inoffset + x + 1]; // g
				outputIMG[outoffset + x + 2]	= inputIMG[inoffset + x + 2]; // b
			}
			inoffset  += width * 3;
			outoffset -= width * 3;
		}
	}

	// vertical && horizontal flipping
	// reversing the order of the pixel
	if(vflip == 1 && hflip == 1) {
		int inoffset  = 0;
		int outoffset = (height * width * 3) - 3; // very last element of the array

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width * 3; x += 3) {
				outputIMG[outoffset - x]		= inputIMG[inoffset + x]; // r
				outputIMG[outoffset - x + 1]	= inputIMG[inoffset + x + 1]; // g
				outputIMG[outoffset - x + 2]	= inputIMG[inoffset + x + 2]; // b
			}
			inoffset  += width * 3;
			outoffset -= width * 3;
		}
	}
	}

	return 0;
}

