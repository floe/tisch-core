/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Filter.h"

/*==============================================================================
 * BGSubFilter
==============================================================================*/
BGSubFilter::BGSubFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	invert = 0;
	adaptive = 0;
	background = new ShortImage( image->getWidth(), image->getHeight() );
	config->QueryIntAttribute( "Invert",   &invert   );
	config->QueryIntAttribute( "Adaptive", &adaptive );
	// setting variables for Configurator
	countOfOptions = 2; // quantity of variables that can be manipulated
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

const char* BGSubFilter::getOptionName(int option) {
	const char* OptionName = "";

	switch(option) {
	case 0:
		OptionName = "Invert";
		break;
	case 1:
		OptionName = "Adaptive";
		break;
	default:
		// leave OptionName empty
		break;
	}

	return OptionName;
}

double BGSubFilter::getOptionValue(int option) {
	double OptionValue = -1.0;

	switch(option) {
	case 0:
		OptionValue = invert;
		break;
	case 1:
		OptionValue = adaptive;
		break;
	default:
		// leave OptionValue = -1.0
		break;
	}

	return OptionValue;
}

void BGSubFilter::modifyOptionValue(double delta) {
	switch(toggle) {
	case 0:
		invert = (invert + 1) % 2; // boolean value
		break;
	case 1:
		adaptive = (adaptive + 1) % 2; // boolean value
		break;
	}
}

/*==============================================================================
 * FlipFilter
==============================================================================*/
// TODO: make hflip/vflip configurable
FlipFilter::FlipFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	hflip = 0;
	vflip = 0;
	// setting variables for Configurator
	countOfOptions = 2; // quantity of variables that can be manipulated
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

void FlipFilter::modifyOptionValue(double delta) {
	switch(toggle) {
	case 0:
		hflip = (hflip + 1) % 2; // boolean value
		break;
	case 1:
		vflip = (vflip + 1) % 2; // boolean value
		break;
	}
}

/*==============================================================================
 * ThreshFilter
==============================================================================*/
// TODO: use result from bgsub filter for threshold adjustment
ThreshFilter::ThreshFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	threshold_min = 128;
	threshold_max = 255;
	config->QueryIntAttribute(      "Threshold", &threshold_min );
	config->QueryIntAttribute( "LowerThreshold", &threshold_min );
	config->QueryIntAttribute( "UpperThreshold", &threshold_max );
	// setting variables for Configurator
	countOfOptions = 2; // quantity of variables that can be manipulated
}

int ThreshFilter::process() {
	input->getImage()->threshold( threshold_min, *image, threshold_max );
	return 0;
}

const char* ThreshFilter::getOptionName(int option) {
	const char* OptionName = "";

	switch(option) {
	case 0:
		OptionName = "Threshold Min";
		break;
	case 1:
		OptionName = "Threshold Max";
		break;
	default:
		// leave OptionName empty
		break;
	}

	return OptionName;
}

double ThreshFilter::getOptionValue(int option) {
	double OptionValue = -1.0;

	switch(option) {
	case 0:
		OptionValue = threshold_min;
		break;
	case 1:
		OptionValue = threshold_max;
		break;
	default:
		// leave OptionValue = -1.0
		break;
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

/*==============================================================================
 * SpeckleFilter
==============================================================================*/
SpeckleFilter::SpeckleFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	noiselevel = 7;
	config->QueryIntAttribute( "NoiseLevel", &noiselevel );
	// setting variables for Configurator
	countOfOptions = 1; // quantity of variables that can be manipulated
}

int SpeckleFilter::process() {
	input->getImage()->despeckle( *image, noiselevel );
	return 0;
}

const char* SpeckleFilter::getOptionName(int option) {
	const char* OptionName = "";

	switch(option) {
	case 0:
		OptionName = "Noiselevel";
		break;
	default:
		// leave OptionName empty
		break;
	}

	return OptionName;
}

double SpeckleFilter::getOptionValue(int option) {
	// only one variable to manipulate, so return it
	return noiselevel;
}

void SpeckleFilter::modifyOptionValue(double delta) {
	switch(toggle) {
	case 0:
		noiselevel += delta;
		noiselevel = (noiselevel < 0) ? 0 : (noiselevel > 7) ? 7 : noiselevel;
		break;
	}
}
/*==============================================================================
 * LowpassFilter
==============================================================================*/
LowpassFilter::LowpassFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	mode = 0;
	range = 1;
	config->QueryIntAttribute( "Mode", &mode);
	config->QueryIntAttribute( "Range", &range);
	// setting variables for Configurator
	countOfOptions = 2; // quantity of variables that can be manipulated
}

int LowpassFilter::process() {
	input->getImage()->lowpass( *image, range, mode );
	return 0;
}

const char* LowpassFilter::getOptionName(int option) {
	const char* OptionName = "";

	switch(option) {
	case 0:
		OptionName = "Mode";
		break;
	case 1:
		OptionName = "Range";
		break;
	default:
		// leave OptionName empty
		break;
	}

	return OptionName;
}

double LowpassFilter::getOptionValue(int option) {
	double OptionValue = -1.0;

	switch(option) {
	case 0:
		OptionValue = mode;
		break;
	case 1:
		OptionValue = range;
		break;
	default:
		// leave OptionValue = -1.0
		break;
	}

	return OptionValue;
}

void LowpassFilter::modifyOptionValue(double delta) {
	switch(toggle) {
	case 0: // mode: 0,1,2
		mode = (mode + 1) % 3;
		break;
	case 1: // range 0 ... MAX_VALUE
		range += delta;
		range = (range < 0) ? 0 : (range > 65535) ? 65535 : range;
		break;
	}
}

/*==============================================================================
 * SplitFilter
==============================================================================*/
SplitFilter::SplitFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	image2 = NULL;
	reset();
	// setting variables for Configurator
	countOfOptions = 0; // quantity of variables that can be manipulated
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
