/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Filter.h"


BGSubFilter::BGSubFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	background = new ShortImage( image->getWidth(), image->getHeight() );
	config->QueryIntAttribute( "Invert",   &invert   );
	config->QueryIntAttribute( "Adaptive", &adaptive );
}

BGSubFilter::~BGSubFilter() {
	delete background;
}

void BGSubFilter::link( Filter* _mask ) {
	mask = _mask;
}

void BGSubFilter::reset() {
	*background = *(input->getImage());
}

int BGSubFilter::process() {
	IntensityImage* inputimg = input->getImage();
	background->subtract( *(inputimg), *image, invert );
	if (adaptive) background->update( *(inputimg), *(mask->getImage()) );
	result = background->intensity(); // FIXME: does 'invert' have to be factored in here?
	return 0;
}


// TODO: make hflip/vflip configurable
FlipFilter::FlipFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
}

// TODO: should be MMX-accelerated
int FlipFilter::process() {

	unsigned char* inbuf  = input->getImage()->getData();
	unsigned char* outbuf = image->getData();

	int width  = image->getWidth();
	int height = image->getHeight();

	int inoffset  = 0;
	int outoffset = width-1;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) outbuf[outoffset-x] = inbuf[inoffset+x];
		inoffset  += width;
		outoffset += width;
	}

	return 0;
}


// TODO: use result from bgsub filter for threshold adjustment
ThreshFilter::ThreshFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	threshold_min = 128;
	threshold_max = 255;
	config->QueryIntAttribute(      "Threshold", &threshold_min );
	config->QueryIntAttribute( "LowerThreshold", &threshold_min );
	config->QueryIntAttribute( "UpperThreshold", &threshold_max );
	toggle = 0;
	countOfOptions = 2; // number of variables that can be manipulated: Min/Max Threshold
}

int ThreshFilter::process() {
	input->getImage()->threshold( threshold_min, *image, threshold_max );
	return 0;
}

void ThreshFilter::nextOption() {
	toggle = (toggle + 1) % countOfOptions;
}

int ThreshFilter::getCurrentOption() {
	return toggle;
}

const int ThreshFilter::getOptionCount() {
	return countOfOptions;
}

const char* ThreshFilter::getOptionName(int option) {
	const char* OptionName = "";
	if(option >= 0 && option < countOfOptions) {
		switch(option) {
		case 0:
			OptionName = "Threshold Min";
			break;
		case 1:
			OptionName = "Threshold Max";
			break;
		}
	}
	return OptionName;
}

double ThreshFilter::getOptionValue(int option) {
	double OptionValue = -1.0;
	if(option >= 0 && option < countOfOptions) {
		switch(option) {
		case 0:
			OptionValue = threshold_min;
			break;
		case 1:
			OptionValue = threshold_max;
			break;
		}
	}
	return OptionValue;
}

void ThreshFilter::modifyOptionValue(double delta) {
	switch(toggle) {
	case 0:
		threshold_min += delta;
		threshold_min = (threshold_min < 0) ? 0 : (threshold_min > 255) ? 255 : threshold_min;
		break;
	case 1:
		threshold_max += delta;
		threshold_max = (threshold_max < 0) ? 0 : (threshold_max > 255) ? 255 : threshold_max;
		break;
	}
}


SpeckleFilter::SpeckleFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	noiselevel = 7;
	config->QueryIntAttribute( "NoiseLevel", &noiselevel );
}

int SpeckleFilter::process() {
	input->getImage()->despeckle( *image, noiselevel );
	return 0;
}

LowpassFilter::LowpassFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	mode = 0;
	range = 1;
	config->QueryIntAttribute( "Mode", &mode);
	config->QueryIntAttribute( "Range", &range);
}

int LowpassFilter::process() {
	input->getImage()->lowpass( *image, range, mode );
	return 0;
}


SplitFilter::SplitFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	image2 = NULL;
	reset();
}

void SplitFilter::reset() {
	incount = outcount = 0;
}	

int SplitFilter::process() {
	incount++;
	if (incount % 2) {
		*image = *(input->getImage());
		return 1;
	} else {
		image2 = input->getImage();
		return 0;
	}
}

// TODO: add intensity heuristic (e.g. 2nd image is the brighter one)
IntensityImage* SplitFilter::getImage() {
	outcount++;
	if (outcount % 2) return image;
	else return image2 ? image2 : image;
}

