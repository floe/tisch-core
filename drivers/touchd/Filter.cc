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
	if(useIntensityImage) background = new ShortImage( image->getWidth(), image->getHeight() );
	else background = new ShortImage( shortimage->getWidth(), shortimage->getHeight() );
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
	if(useIntensityImage) *background = *(input->getImage());
	else *background = *(input->getShortImage());
}

int BGSubFilter::process() {
	if(useIntensityImage) 
	{
		IntensityImage* inputimg = input->getImage();
		background->subtract( *(inputimg), *image, invert );
		if (adaptive) background->update( *(inputimg), *(mask->getImage()) );
		result = background->intensity(); // FIXME: does 'invert' have to be factored in here?
	}
	else
	{
		ShortImage* inputimg = input->getShortImage();
		background->subtract( *(inputimg), *shortimage, invert );
		if (adaptive) background->update( *(inputimg), *(mask->getImage()) );
		result = background->intensity(); // FIXME: does 'invert' have to be factored in here?
	}
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

void BGSubFilter::modifyOptionValue(double delta, bool overwrite) {
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
	
	if(useIntensityImage) 
	{
		unsigned char* inbuf = input->getImage()->getData();
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
	} 
	else 
	{
		unsigned short* inbuf  = input->getShortImage()->getSData();
		unsigned short* outbuf = shortimage->getSData();

		int width  = shortimage->getWidth();
		int height = shortimage->getHeight();

		int inoffset  = 0;
		int outoffset = width-1;

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) outbuf[outoffset-x] = inbuf[inoffset+x];
			inoffset  += width;
			outoffset += width;
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
	(useIntensityImage == 1) ? THRESH_MAX = 255 : THRESH_MAX = 2047;
	// setting variables for Configurator
	countOfOptions = 2; // quantity of variables that can be manipulated
}

int ThreshFilter::process() {
	if(useIntensityImage) input->getImage()->threshold( threshold_min, *image, threshold_max );
	else input->getShortImage()->threshold( threshold_min << 5, *shortimage, threshold_max << 5 );
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

void ThreshFilter::modifyOptionValue(double delta, bool overwrite) {
	switch(toggle) {
	case 0:
		if(overwrite) {
			threshold_min = (delta < 0) ? 0 : (delta > THRESH_MAX) ? THRESH_MAX : delta;
		} else {
			threshold_min += delta;
			threshold_min = (threshold_min < 0) ? 0 : (threshold_min > THRESH_MAX) ? THRESH_MAX : threshold_min;
		}
		break;
	case 1:
		if(overwrite) {
			threshold_max = (delta < 0) ? 0 : (delta > THRESH_MAX) ? THRESH_MAX : delta;
		} else {
			threshold_max += delta;
			threshold_max = (threshold_max < 0) ? 0 : (threshold_max > THRESH_MAX) ? THRESH_MAX : threshold_max;
		}
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
	if(useIntensityImage) input->getImage()->despeckle( *image, noiselevel );
	else input->getShortImage()->despeckle( *shortimage, noiselevel );
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

void SpeckleFilter::modifyOptionValue(double delta, bool overwrite) {
	switch(toggle) {
	case 0:
		if(overwrite) {
			noiselevel = (delta < 0) ? 0 : (delta > 7) ? 7 : delta;
		} else {
			noiselevel += delta;
			noiselevel = (noiselevel < 0) ? 0 : (noiselevel > 7) ? 7 : noiselevel;
		}
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
	if(useIntensityImage) input->getImage()->lowpass( *image, range, mode );
	else input->getShortImage()->lowpass( *shortimage, range, mode );
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

void LowpassFilter::modifyOptionValue(double delta, bool overwrite) {
	switch(toggle) {
	case 0: // mode: 0,1,2
		if(overwrite) {
			mode = (delta < 0) ? 0 : (delta > 2) ? 2 : delta;
		} else {
			mode += delta;
			mode = (mode < 0) ? 0 : (mode > 2) ? 2 : mode;
		}
		break;
	case 1: // range 0 ... MAX_VALUE
		if(overwrite) {
			range = (delta < 0) ? 0 : (delta > MAX_VALUE) ? MAX_VALUE : delta;
		} else {
			range += delta;
			range = (range < 0) ? 0 : (range > MAX_VALUE) ? MAX_VALUE : range;
		}
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

/*==============================================================================
 * AreaFilter
==============================================================================*/
AreaFilter::AreaFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	enabled = 0;
	updated = true;
	config->QueryIntAttribute( "Enabled", &enabled);
	// setting variables for Configurator
	countOfOptions = 1; // quantity of variables that can be manipulated
}

int AreaFilter::process() {
	if(enabled)
		if(useIntensityImage) input->getImage()->areamask( *image, edgepoints );
		else input->getShortImage()->areamask( *shortimage, edgepoints );
	else
		if(useIntensityImage) *image = *(input->getImage());
		else *shortimage = *(input->getShortImage());
	return 0;
}

void AreaFilter::processMouseButton( int button, int state, int x, int y )
{
	//add a cornerpoint to the newest polygon
	if(button == 0 && state == 0)
	{
		if(updated) 
		{
			cornerpointvector.push_back(std::vector<Point*>());
			updated = false;
		}
		Point* a = new Point();
		a->x = x;
		a->y = y;
		(*(cornerpointvector.end() - 1)).push_back(a);
	}

	//update polygons
	if(button == 2 && state == 0 && !updated)
	{
		updated = true;
		edgepoints.clear();

		if(cornerpointvector.empty()) return;

		for(std::vector<std::vector<Point*>>::iterator it = cornerpointvector.begin(); it != cornerpointvector.end(); it++)
		{
			if((*it).empty()) continue;
			if(*((*it).begin()) != *((*it).end()-1)) (*it).push_back(*(*it).begin()); //copies the first point of a polygon to the end of its pointvector
			generateEdgepoints(*it);
		}
		std::sort(edgepoints.begin(), edgepoints.end()); //sort the list for later use
	}
}

//generates all edgepoints between subsequent cornerpoints
void AreaFilter::generateEdgepoints( std::vector<Point*> cornerpoints )
{
	for(std::vector<Point*>::iterator it = cornerpoints.begin(); it != cornerpoints.end()-1; it++)
	{	
		int max = fabs((double)((*it)->y - (*(it + 1))->y));
		if( max != 0)
			for(int i = 0; i < max; i++)
			{
				double helpx = (*it)->x + ( i * ((*(it + 1))->x - (*it)->x) / max );
				double helpy = (((*(it + 1))->y - (*it)->y) < 0 ? -1 : 1) * (i + 0.5) + (*it)->y;
				edgepoints.push_back((helpy-0.5)*image->getWidth() + helpx); //converts coordinates into data indices
			}
	}
}

void AreaFilter::reset()
{
	edgepoints.clear();
	cornerpointvector.clear();
	updated = true;
}

const char* AreaFilter::getOptionName(int option) {
	const char* OptionName = "";

	switch(option) {
	case 0:
		OptionName = "Enabled";
		break;
	default:
		// leave OptionName empty
		break;
	}

	return OptionName;
}

double AreaFilter::getOptionValue(int option) {
	double OptionValue = -1.0;

	switch(option) {
	case 0:
		OptionValue = enabled;
		break;
	default:
		// leave OptionValue = -1.0
		break;
	}

	return OptionValue;
}

void AreaFilter::modifyOptionValue(double delta, bool overwrite) {
	switch(toggle) {
	case 0: 
		enabled = (enabled + 1) % 2; //boolean value 
		break;
	}
}

void AreaFilter::draw( GLUTWindow* win )
{ 
	if(useIntensityImage) win->show( *image, 0, 0 ); 
	else win->show( *shortimage, 0, 0 );
	glColor4f(1,0,0,1);
	for(std::vector<std::vector<Point*>>::iterator it = cornerpointvector.begin(); it != cornerpointvector.end(); it++)
		win->drawPolygon( *it, 1, image->getHeight() );
}
