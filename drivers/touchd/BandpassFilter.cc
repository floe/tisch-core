/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BandpassFilter.h"

BandpassFilter::BandpassFilter( TiXmlElement* _config, Filter* _input ):
	Filter( _config, _input, FILTER_TYPE_BASIC)
{
	inner = 8;
	outer = 16;
	config->QueryIntAttribute( "InnerRadius", &inner );
	config->QueryIntAttribute( "OuterRadius", &outer );
	// setting variables for Configurator
	countOfOptions = 2; // quantity of variables that can be manipulated
}

int BandpassFilter::process() {
	if(image) input->getImage()->bandpass( *image, outer, inner );
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
