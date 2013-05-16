/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "MarkerTrackerFilter.h"

#ifdef HAS_UBITRACK

MarkerTrackerFilter::MarkerTrackerFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	
	config->QueryIntAttribute( "MarkerTracker", &int_mt_enabled );
	config->QueryIntAttribute( "MTshowMarker", &int_mt_showMarker );

	// check values for value range and set default values
	(int_mt_enabled == 1)		? mt_enabled = true		: mt_enabled = false;
	(int_mt_showMarker == 1)	? mt_showMarker = true	: mt_showMarker = false;

	// create MarkerTracker with read settings
	mMarkerTracker = new MarkerTracker( _input->getImage()->getWidth(), _input->getImage()->getHeight() );
	mMarkerTracker->addMarkerID(0xb44);
	mMarkerTracker->addMarkerID(0x272);

	detectedMarkers = new std::vector<Ubitrack::Vision::SimpleMarkerInfo>;
	
	// setting variables for Configurator
	countOfOptions = 2; // quantity of variables that can be manipulated

}

MarkerTrackerFilter::~MarkerTrackerFilter() {
	delete mMarkerTracker;
	delete detectedMarkers;
}

int MarkerTrackerFilter::process() {

	//shortimage = input->getShortImage();
	// MarkerTrackFilter needs no SplitFilter to seperate IntensityImage and RGBImage
	// to get access to RGBImage.
	rgbimage = input->getRGBImage(); // get pointer from previous filter, do nothing
	image = input->getImage();

	if( mt_enabled ) {
		// call MarkerTracker here
		mMarkerTracker->findMarker(rgbimage, image, detectedMarkers);

	}

	return 0;
}

void MarkerTrackerFilter::draw( GLUTWindow* win ) {

	if( displayRGBImage )
		win->show( *rgbimage, 0, 0);
	else
		win->show( *image, 0, 0);

	if( mt_enabled ) {
		glColor4f( 1.0, 0.0, 0.0, 1.0 );
		win->print( std::string("makertracker running"), 10, win->getHeight() - 30 );
	}

	if( mt_enabled && mt_showMarker ) {
		
		glColor4f( 1.0, 0.0, 0.0, 1.0 );

		std::ostringstream MarkerID;
		MarkerID.str("");
		MarkerID << "marker ";
		
		std::vector<Ubitrack::Vision::SimpleMarkerInfo>::iterator iter;
		for(iter = detectedMarkers->begin(); iter < detectedMarkers->end(); iter++) {
			MarkerID << std::hex << setfill('0') << setw(2) << nouppercase << iter->ID << " ";
		}

		win->print( MarkerID.str(), 10, win->getHeight() - 50  );
		
	}
	
}

const char* MarkerTrackerFilter::getOptionName(int option) {
	const char* OptionName = "";

	switch(option) {
	case 0:
		OptionName = "MT enabled";
		break;
	case 1:
		OptionName = "MT show Marker";
		break;
	default:
		// leave OptionName empty
		break;
	}

	return OptionName;
}

double MarkerTrackerFilter::getOptionValue(int option) {
	double OptionValue = -1.0;

	switch(option) {
	case 0:
		OptionValue = mt_enabled;
		break;
	case 1:
		OptionValue = mt_showMarker;
		break;
	default:
		// leave OptionValue = -1.0
		break;
	}

	return OptionValue;
}

void MarkerTrackerFilter::modifyOptionValue(double delta, bool overwrite) {
	switch(toggle) {
	case 0: // mt_enabled is a boolean value
		if(overwrite) {
			mt_enabled = (delta == 0 ? 0 : (delta == 1 ? 1 : mt_enabled));
		} else {
			mt_enabled += delta;
			mt_enabled = (mt_enabled < 0) ? 0 : (mt_enabled > 1) ? 1 : mt_enabled;
		}
		break;
	case 1: // mt_showMarker is a boolean value
		if(overwrite) {
			mt_showMarker = (delta == 0 ? 0 : (delta == 1 ? 1 : mt_showMarker));
		} else {
			mt_showMarker += delta;
			mt_showMarker = (mt_showMarker < 0) ? 0 : (mt_showMarker > 1) ? 1 : mt_showMarker;
		}
		break;
	
	}
}

TiXmlElement* MarkerTrackerFilter::getXMLRepresentation() {
	
	TiXmlElement* XMLNode = new TiXmlElement( "MarkerTrackerFilter" );
	
	XMLNode->SetAttribute( "MarkerTracker", mt_enabled );
	XMLNode->SetAttribute( "MTshowMarker", mt_showMarker );
	
	return XMLNode;
}

#endif // HAS_UBITRACK
