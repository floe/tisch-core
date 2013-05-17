/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "FlipFilter.h"

FlipFilter::FlipFilter( TiXmlElement* _config, Filter* _input ):
	Filter( _config, _input, FILTER_TYPE_ALL )
{
	hflip = 0;
	vflip = 0;
	config->QueryIntAttribute( "HFlip" , &hflip );
	config->QueryIntAttribute( "VFlip" , &vflip );
	// setting variables for Configurator
	countOfOptions = 2; // quantity of variables that can be manipulated
}

// TODO: should be MMX-accelerated
int FlipFilter::process() {

	int width = 0;
	int height = 0;

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
	else 
	{
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

	return 0;
}

const char* FlipFilter::getOptionName(int option) {
	const char* OptionName = "";

	switch(option) {
	case 0:
		OptionName = "Horizontal Flip";
		break;
	case 1:
		OptionName = "Vertical Flip";
		break;
	default:
		// leave OptionName empty
		break;
	}

	return OptionName;
}

double FlipFilter::getOptionValue(int option) {
	double OptionValue = -1.0;

		switch(option) {
		case 0:
			OptionValue = hflip;
			break;
		case 1:
			OptionValue = vflip;
			break;
		default:
			// leave OptionValue = -1.0
			break;
		}

		return OptionValue;
}

void FlipFilter::modifyOptionValue(double delta, bool overwrite) {
	switch(toggle) {
	case 0: // hflip is a boolean value
		if(overwrite) {
			hflip = (delta == 0 ? 0 : (delta == 1 ? 1 : hflip));
		} else {
			hflip += delta;
			hflip = (hflip < 0) ? 0 : (hflip > 1) ? 1 : hflip;
		}
		break;
	case 1: // vflip is a boolean value
		if(overwrite) {
			vflip = (delta == 0 ? 0 : (delta == 1 ? 1 : vflip));
		} else {
			vflip += delta;
			vflip = (vflip < 0) ? 0 : (vflip > 1) ? 1 : vflip;
		}
		break;
	}
}

TiXmlElement* FlipFilter::getXMLRepresentation() {
	TiXmlElement* XMLNode = new TiXmlElement( "FlipFilter" );
	
	XMLNode->SetAttribute( "HFlip" , hflip );
	XMLNode->SetAttribute( "VFlip" , vflip );
	
	return XMLNode;
}
