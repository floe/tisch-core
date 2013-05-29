/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BGSubFilter.h"

BGSubFilter::BGSubFilter( TiXmlElement* _config, Filter* _input ):
	Filter( _config, _input, FILTER_TYPE_GREY )
{
	resetOnInit = 1;
	if (image) background = new ShortImage( image->getWidth(), image->getHeight() );
	else background = new ShortImage( shortimage->getWidth(), shortimage->getHeight() );
	createOption( "Invert", 0, 0, 1 );
	createOption( "Adaptive", 0, 0, 1 );
	createOption( "storeBGImg", 0, 0, 1 );
	loadFilterOptions( _config, true );
}

BGSubFilter::~BGSubFilter() {
	delete background;
}

void BGSubFilter::link( Filter* _mask ) {
	mask = _mask;
}

void BGSubFilter::reset(int initialReset) {
	if( (initialReset == 1 && resetOnInit == 1) || initialReset == 0 ) {
		if (image) *background = *(input->getImage());
		else *background = *(input->getShortImage());
	}
}

int BGSubFilter::process() {
	int invert = options["Invert"]->get();
	int adaptive = options["Adaptive"]->get();
	if(image) 
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


TiXmlElement* BGSubFilter::getXMLRepresentation() {
	TiXmlElement* XMLNode = Filter::getXMLRepresentation();
	TiXmlElement* bg_node = getXMLofBackground(123,"foo");
	XMLNode->LinkEndChild( bg_node );
	return XMLNode;
}

TiXmlElement* BGSubFilter::getXMLofBackground(int BGSubFilterID, std::string path) {
	TiXmlElement* XMLNodeBG = new TiXmlElement( "BGSubFilter" );

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
	} else {
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
		if(type == "BGImage") {
			std::string filename;
			filterOption->QueryStringAttribute( "BGImgPath", &filename);
			resetOnInit = loadPGMImageFromFile( filename , debug);
			break;
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
