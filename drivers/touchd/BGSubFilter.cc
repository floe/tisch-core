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
	loadFilterOptions( _config );
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
	if (bg_node) XMLNode->LinkEndChild( bg_node );
	return XMLNode;
}

TiXmlElement* BGSubFilter::getXMLofBackground(int BGSubFilterID, std::string path) {
	TiXmlElement* XMLNodeBG = new TiXmlElement( "Options" );

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

		background->save(BGImg.c_str());

		XMLNodeBG->SetAttribute( "BGImgPath" , BGImg );
		return XMLNodeBG;
	}

	XMLNodeBG->SetAttribute( "something went wrong" , -1 );
	return XMLNodeBG;

}

void BGSubFilter::loadFilterOptions( TiXmlElement* OptionSubtree ) {

	TiXmlElement* filterOption = OptionSubtree->FirstChildElement();
	do {
		// iterate through all children of OptionSubtree
		std::string type = filterOption->Value();
		if (type == "Options") {
			std::string filename;
			filterOption->QueryStringAttribute( "BGImgPath", &filename);
			resetOnInit = loadPGMImageFromFile( filename );
			break;
		}
	} while((filterOption = filterOption->NextSiblingElement()));
}

int BGSubFilter::loadPGMImageFromFile( std::string filename ) {

	try {
		ShortImage* oldbg = background;
		background = new ShortImage(filename.c_str());
		delete oldbg;
	} catch(...) {
		return 1;
	}
	
	return 0; // everything OK
}
