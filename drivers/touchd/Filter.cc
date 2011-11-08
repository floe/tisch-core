/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
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
	resetOnInit = 1;
	BGSubFilterID = -1; // -1 is invalid
	storeBGImg = 0;
	if(useIntensityImage) background = new ShortImage( image->getWidth(), image->getHeight() );
	else background = new ShortImage( shortimage->getWidth(), shortimage->getHeight() );
	config->QueryIntAttribute( "BGSubFilterID", &BGSubFilterID );
	config->QueryIntAttribute( "Invert",   &invert   );
	config->QueryIntAttribute( "Adaptive", &adaptive );
	config->QueryIntAttribute( "storeBGImg", &storeBGImg );
	// setting variables for Configurator
	countOfOptions = 3; // quantity of variables that can be manipulated
}

BGSubFilter::~BGSubFilter() {
	delete background;
}

void BGSubFilter::link( Filter* _mask ) {
	mask = _mask;
}

void BGSubFilter::reset(int initialReset) {
	if( (initialReset == 1 && resetOnInit == 1) || initialReset == 0 ) {
		if(useIntensityImage) *background = *(input->getImage());
		else *background = *(input->getShortImage());
	}
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
	case 2:
		OptionName = "store BGImg";
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
	case 2:
		OptionValue = storeBGImg;
		break;
	default:
		// leave OptionValue = -1.0
		break;
	}

	return OptionValue;
}

void BGSubFilter::modifyOptionValue(double delta, bool overwrite) {
	switch(toggle) {
	case 0: // invert is a boolean value
		if(overwrite) {
			invert = (delta == 0 ? 0 : (delta == 1 ? 1 : invert));
		} else {
			invert += delta;
			invert = (invert < 0) ? 0 : (invert > 1) ? 1 : invert;
		}
		break;
	case 1: // adaptive is a boolean value
		if(overwrite) {
			adaptive = (delta == 0 ? 0 : (delta == 1 ? 1 : adaptive));
		} else {
			adaptive += delta;
			adaptive = (adaptive < 0) ? 0 : (adaptive > 1) ? 1 : adaptive;
		}
		break;
	case 2: // overwrite is boolean value
		if(overwrite) {
			storeBGImg = (delta == 0 ? 0 : (delta == 1 ? 1 : storeBGImg));
		}
		else {
			storeBGImg += delta;
			storeBGImg = (storeBGImg < 0) ? 0 : (storeBGImg > 1) ? 1 : storeBGImg;
		}
	}
}

TiXmlElement* BGSubFilter::getXMLRepresentation() {
	
	TiXmlElement* XMLNode = new TiXmlElement( "BGSubFilter" );
	
	XMLNode->SetAttribute( "BGSubFilterID" , BGSubFilterID );
	XMLNode->SetAttribute( "Invert" , invert );
	XMLNode->SetAttribute( "Adaptive" , adaptive );
	XMLNode->SetAttribute( "storeBGImg" , storeBGImg );
	
	return XMLNode;
}

int BGSubFilter::getBGSubFilterID() {
	return BGSubFilterID;
}

TiXmlElement* BGSubFilter::getXMLofBackground(int BGSubFilterID, std::string path) {
	TiXmlElement* XMLNodeBG = new TiXmlElement( "BGSubFilter" );
	XMLNodeBG->SetAttribute( "BGSubFilterID" , BGSubFilterID );

	// filename
	std::ostringstream filename;
	filename.str("");
	filename << "BGSubFilter_ID_" << BGSubFilterID << "_Background.pgm";

	// store location
	std::string BGImg = path.append(filename.str());

	// check whether file already exists
    std::fstream testExistance(BGImg.c_str(), std::ios::in);
    if (testExistance.good()) {
		// file already exists
        testExistance.close();
        if( storeBGImg == 0 ) {
			// don't overwrite BGImg
			XMLNodeBG->SetAttribute( "BGImgPath" , BGImg );
			return XMLNodeBG;
		}
    }
	else {
		// doesnt exist
		if( storeBGImg == 0 ) {
			// don't store
			return 0;
		}
    } 

	// store/overwrite BG Image only if storing for this BGSubfilter is enabled
	if(storeBGImg == 1) {

		std::ofstream bgimage(BGImg.c_str(), std::ios::out);

		// filecontent
		bgimage << "P2\n";
		bgimage << "# CREATOR: libTISCH version 2.0\n";
		bgimage << background->getWidth() << " " << background->getHeight() << "\n";
		bgimage << "65535\n"; // a ShortImage can store 2^16 Bit (2Byte) per pixel
	
		// write background image pixel by pixel, for each one line
		for( int y = 0; y < background->getHeight(); y++) {
			for (int x = 0; x < background->getWidth(); x++) {
				bgimage << background->getPixel(x,y) << "\n";
			}
		}
	
		// be polite and tidy up ;)
		bgimage.close();

		XMLNodeBG->SetAttribute( "BGImgPath" , BGImg );
		return XMLNodeBG;
	}

	XMLNodeBG->SetAttribute( "something went wrong" , -1 );
	return XMLNodeBG;

}

void BGSubFilter::loadFilterOptions(TiXmlElement* OptionSubtree, bool debug) {
	std::cout << "reading stored options for BGSubFilter from config ... ";
	if(debug)
		std::cout << std::endl;

	TiXmlElement* filterOption = OptionSubtree->FirstChildElement();
	do {
		// iterate through all children of OptionSubtree
		std::string type = filterOption->Value();
		if(type == "BGSubFilter") {
			int filterID = -1;
			filterOption->QueryIntAttribute( "BGSubFilterID" , &filterID );
			if( (filterID == BGSubFilterID) && (filterID != -1) ) {
				// settings are for the current BGSubFilter

				if(debug)
					std::cout << "BGSubFilterID: " << BGSubFilterID << std::endl;

				std::string filename;
				filterOption->QueryStringAttribute( "BGImgPath", &filename);
				resetOnInit = loadPGMImageFromFile( filename , debug);
				break;
			}
		}
	} while((filterOption = filterOption->NextSiblingElement()));

	// TODO: read Background Image here and store it to background

	std::cout << "done" << std::endl;
}

int BGSubFilter::loadPGMImageFromFile(std::string filename, bool debug) {
	if(debug)
		std::cout << "reading pixel values from " << filename << std::endl;

	std::ifstream inputFile;
	inputFile.open(filename.c_str());

	if(!inputFile) {
		std::cout << " something went wrong reading pgm file! check format!" << std::endl;
		return 1;
	}

	int x = 0;
	int y = 0;
	int height;
	int width;
	std::string line;

	// Fileheader: we expect pgm file format
	// PGM magic number
	inputFile >> line;
	if(strcmp(line.c_str(), "P2") != 0) {
		if(debug)
			std::cout << "provided file has wrong magic number: P2 expected" << std::endl;
		return 1;
	}

	// Creator comment
	inputFile >> line;
	
	// width x height
	inputFile >> line;
	size_t found;
	found = line.find(" ");
	if (found != std::string::npos) {
		width = atoi(line.substr(0,found).c_str());
		height = atoi(line.substr(found+1).c_str());
	}
    
	// colour depth
	inputFile >> line;

	// read the content line by line, so pixel by pixel
	while(inputFile >> line) {
		int pixelVal = atoi(line.c_str());

		background->setPixel(x, y, pixelVal);
		if(x + 1 < width)
			x++;
		else {
			x = 0;
			if(y + 1 < height)
				y++;
			else 
				std::cout << "pixel position out of range" << std::endl;
		}
		
	}

	// be polite and tidy up ;)
	inputFile.close();
	
	return 0; // everything OK
}
/*==============================================================================
 * FlipFilter
==============================================================================*/
FlipFilter::FlipFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
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

	if(useIntensityImage) 
	{
		unsigned char* inbuf = input->getImage()->getData();
		unsigned char* outbuf = image->getData();

		width  = image->getWidth();
		height = image->getHeight();

		// no flipping
		if (vflip == 0 && hflip == 0) {
			int inoffset  = 0;
			int outoffset = 0;

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) outbuf[outoffset + x] = inbuf[inoffset + x];
				inoffset  += width;
				outoffset += width;
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

		// vertical flipping - flipping along horzontal axis
		if(vflip == 1 && hflip == 0){
			int inoffset  = 0;
			int outoffset = (height * width) - (width + 2); // beginning of the last line

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
		unsigned short* inbuf  = input->getShortImage()->getSData();
		unsigned short* outbuf = shortimage->getSData();

		width  = shortimage->getWidth();
		height = shortimage->getHeight();

		// no flipping
		if (vflip == 0 && hflip == 0) {
			int inoffset  = 0;
			int outoffset = 0;

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) outbuf[outoffset + x] = inbuf[inoffset + x];
				inoffset  += width;
				outoffset += width;
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
			int outoffset = (height * width) - (width + 2); // beginning of the last line

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
/*==============================================================================
 * ThreshFilter
==============================================================================*/
// TODO: use result from bgsub filter for threshold adjustment
ThreshFilter::ThreshFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	threshold_min = 128;
	threshold_max = 255;
	config->QueryIntAttribute(      "Threshold", &threshold_min ); // TODO remove this when storing xml works
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

TiXmlElement* ThreshFilter::getXMLRepresentation() {
	TiXmlElement* XMLNode = new TiXmlElement( "ThreshFilter" );
	
	XMLNode->SetAttribute( "LowerThreshold", threshold_min );
	XMLNode->SetAttribute( "UpperThreshold", threshold_max );
	
	return XMLNode;
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

TiXmlElement* SpeckleFilter::getXMLRepresentation() {
	TiXmlElement* XMLNode = new TiXmlElement( "SpeckleFilter" );
	
	XMLNode->SetAttribute( "NoiseLevel", noiselevel );
	
	return XMLNode;
}
/*==============================================================================
 * LowpassFilter
==============================================================================*/
LowpassFilter::LowpassFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	mode = 0;
	range = 1;
	config->QueryIntAttribute( "Mode", &mode );
	config->QueryIntAttribute( "Range", &range );
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

TiXmlElement* LowpassFilter::getXMLRepresentation() {
	TiXmlElement* XMLNode = new TiXmlElement( "LowpassFilter" );
	
	XMLNode->SetAttribute( "Mode", mode );
	XMLNode->SetAttribute( "Range", range );
	
	return XMLNode;
}
/*==============================================================================
 * BandpassFilter
==============================================================================*/
BandpassFilter::BandpassFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	inner = 8;
	outer = 16;
	config->QueryIntAttribute( "InnerRadius", &inner );
	config->QueryIntAttribute( "OuterRadius", &outer );
	// setting variables for Configurator
	countOfOptions = 2; // quantity of variables that can be manipulated
}

int BandpassFilter::process() {
	if(useIntensityImage) input->getImage()->bandpass( *image, outer, inner );
	//else input->getShortImage()->lowpass( *shortimage, range, mode );
	return 0;
}

const char* BandpassFilter::getOptionName(int option) {
	const char* OptionName = "";

	switch(option) {
	case 0:
		OptionName = "OuterRadius";
		break;
	case 1:
		OptionName = "InnerRadius";
		break;
	default:
		// leave OptionName empty
		break;
	}

	return OptionName;
}

double BandpassFilter::getOptionValue(int option) {
	double OptionValue = -1.0;

	switch(option) {
	case 0:
		OptionValue = outer;
		break;
	case 1:
		OptionValue = inner;
		break;
	default:
		// leave OptionValue = -1.0
		break;
	}

	return OptionValue;
}

void BandpassFilter::modifyOptionValue(double delta, bool overwrite) {
	switch(toggle) {
	case 0: // outer: 4..64
		if(overwrite) {
			outer = (delta < 4) ? 4 : (delta > 64) ? 64 : delta;
		} else {
			outer += delta;
			outer = (outer < 4) ? 4 : (outer > 64) ? 64 : outer;
		}
		break;
	case 1: // inner: 4..64
		if(overwrite) {
			inner = (delta < 4) ? 4 : (delta > 64) ? 64 : delta;
		} else {
			inner += delta;
			inner = (inner < 4) ? 4 : (inner > 64) ? 64 : inner;
		}
		break;
	}
}

TiXmlElement* BandpassFilter::getXMLRepresentation() {
	TiXmlElement* XMLNode = new TiXmlElement( "BandpassFilter" );
	
	XMLNode->SetAttribute( "InnerRadius", inner );
	XMLNode->SetAttribute( "OuterRadius", outer );
	
	return XMLNode;
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

TiXmlElement* SplitFilter::getXMLRepresentation() {
	TiXmlElement* XMLNode = new TiXmlElement( "SplitFilter" );
	
	//XMLNode->SetAttribute( "name", value );
	
	return XMLNode;
}
/*==============================================================================
 * AreaFilter
==============================================================================*/
AreaFilter::AreaFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	enabled = 0;
	updated = true;
	resetOnInit = 1; // is set to 0 if polygons where read from config to edgepoint vector
	AreaFilterID = -1;
	config->QueryIntAttribute( "AreaFilterID", &AreaFilterID );
	config->QueryIntAttribute( "Enabled", &enabled );
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

		for(std::vector<std::vector<Point*> >::iterator it = cornerpointvector.begin(); it != cornerpointvector.end(); it++)
		{
			if((*it).empty()) continue;
			if(*((*it).begin()) != *((*it).end()-1)) (*it).push_back(*(*it).begin()); //copies the first point of a polygon to the end of its pointvector
			generateEdgepoints(*it);
		}
		std::sort(edgepoints.begin(), edgepoints.end()); // sort the list so that the areamask is applied to the right pixelareas
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

void AreaFilter::reset(int initialReset)
{
	if( (initialReset == 1 && resetOnInit == 1) || initialReset == 0 ) {
		edgepoints.clear();
		cornerpointvector.clear();
		updated = true;
	}
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
	case 0: // enabled is a boolean value
		if(overwrite) {
			enabled = (delta == 0 ? 0 : (delta == 1 ? 1 : enabled));
		} else {
			enabled += delta;
			enabled = (enabled < 0) ? 0 : (enabled > 1) ? 1 : enabled;
		}
		break;
	}
}

void AreaFilter::draw( GLUTWindow* win )
{ 
	if(useIntensityImage) win->show( *image, 0, 0 ); 
	else win->show( *shortimage, 0, 0 );
	glColor4f(1,0,0,1);
	for(std::vector<std::vector<Point*> >::iterator it = cornerpointvector.begin(); it != cornerpointvector.end(); it++)
		win->drawPolygon( *it, 1, image->getHeight() );
}

TiXmlElement* AreaFilter::getXMLRepresentation() {
	TiXmlElement* XMLNode = new TiXmlElement( "AreaFilter" );
	
	XMLNode->SetAttribute( "AreaFilterID" , AreaFilterID );
	XMLNode->SetAttribute( "Enabled" , enabled );
	
	return XMLNode;
}

int AreaFilter::getAreaFilterID() {
	return AreaFilterID;
}

TiXmlElement* AreaFilter::getXMLofAreas(int AreaFilterID) {
	
	int polygoncounter = 0;

	TiXmlElement* polygonsOfAreaFilter = new TiXmlElement( "AreaFilter" );
	polygonsOfAreaFilter->SetAttribute( "AreaFilterID" , AreaFilterID );

	// no areas are selected
	if(cornerpointvector.empty())
		return 0;

	for(std::vector<std::vector<Point*> >::iterator iter_polygons = cornerpointvector.begin(); iter_polygons != cornerpointvector.end(); iter_polygons++) {
		// iterate through all polygons

		// ignore polygons with less than 3 edges
		if((*iter_polygons).size() < 3) {
			continue;
		}

		TiXmlElement* polygon = new TiXmlElement( "Polygon" );
		polygon->SetAttribute("number", polygoncounter );

		for(std::vector<Point*>::iterator iter_points = (*iter_polygons).begin(); iter_points != (*iter_polygons).end(); iter_points++) {
			// iterate through all points of current polygon

			// create XML Node and store point values
			TiXmlElement* point = new TiXmlElement( "Point" );
			point->SetAttribute("x", (*iter_points)->x);
			point->SetAttribute("y", (*iter_points)->y);

			// store XML Node in tree
			polygon->LinkEndChild(point);

		} // end iter_points

		polygonsOfAreaFilter->LinkEndChild(polygon);
		polygoncounter++;
		
	} // end iter_polygons

	if(polygonsOfAreaFilter->NoChildren())
		return 0;
	return polygonsOfAreaFilter;
}

void AreaFilter::loadFilterOptions(TiXmlElement* OptionSubtree, bool debug) {
	
	// check OptionSubtree to find settings for current AreaFilter ...
	std::cout << "reading stored areas for AreaFilter from config ... ";
	if(debug)
		 std::cout << std::endl;

	TiXmlElement* filterOption = OptionSubtree->FirstChildElement();
	do {
		std::string type = filterOption->Value();
		if(type == "AreaFilter") {
			// current Options are for an AreaFilter
			int filterID = -1;
			filterOption->QueryIntAttribute( "AreaFilterID" , &filterID);
			if( filterID == AreaFilterID && filterID != -1) {
				// settings are for current AreaFilter

				// filterOption has AreaFilter Subtree -> children are Polygons
				if(debug)
					std::cout << "AreaFilterID: " << AreaFilterID << std::endl;
				
				resetOnInit = createFilterAreaFromConfig(filterOption, debug);
				break;
			}
		}
	} while((filterOption = filterOption->NextSiblingElement()));
	
	std::cout << "done" << std::endl;

}

/*
* returns 0 if edgepoint vector was filled with polygons from config
* else returns 1;
*/
int AreaFilter::createFilterAreaFromConfig(TiXmlElement* PolygonsOfAreaFilter, bool debug) {
	
	// reset cornerpointvector for this areafilter
	cornerpointvector.clear();
	edgepoints.clear();

	// first polygon
	TiXmlElement* Polygon = PolygonsOfAreaFilter->FirstChildElement();

	do {
		// process polygon
		TiXmlElement* Coords = Polygon->FirstChildElement();
		int polygonNr;
		Polygon->QueryIntAttribute("number", &polygonNr);
		
		if(debug)
			std::cout << "Polygon: " << polygonNr << std::endl;
		
		cornerpointvector.push_back(std::vector<Point*>());
					
		do {
			// process Points
			int x_coord;
			int y_coord;

			Coords->QueryIntAttribute("x", &x_coord);
			Coords->QueryIntAttribute("y", &y_coord);
			
			if(debug)
				std::cout << "Point: " << "x: " << x_coord << " / " << "y: " << y_coord << std::endl;
		
			Point* p = new Point();
			p->x = x_coord;
			p->y = y_coord;

			(*(cornerpointvector.end() - 1)).push_back(p);

			// get next point
		} while ((Coords = Coords->NextSiblingElement()));
		
		generateEdgepoints(*(cornerpointvector.end() - 1));
		std::sort(edgepoints.begin(), edgepoints.end());
		
		// get next polygon
	} while((Polygon = Polygon->NextSiblingElement()));

	if(debug) {
		for(std::vector<std::vector<Point*> >::iterator it = cornerpointvector.begin(); it != cornerpointvector.end(); it++) {
			for(std::vector<Point*>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++) {
				std::cout << "x: " << (*(*it2)).x << " y: " << (*it2)->y << std::endl;
			}
		}
	}

	if(edgepoints.size() > 0)
		return 0;
	return 1;
}
