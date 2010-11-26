/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Filter.h"


BGSubFilter::BGSubFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	background = new ShortImage( image->getWidth(), image->getHeight() );
	config->QueryIntAttribute("Invert",&invert);
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

void BGSubFilter::process() {
	background->subtract( *(input->getImage()), *image, invert );
	background->update( *(input->getImage()), *(mask->getImage()) );
	result = background->intensity();
	//std::cout << "bgsub" << std::endl;
}


ThreshFilter::ThreshFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	config->QueryIntAttribute("Threshold",&threshold);
}

void ThreshFilter::process() {
	input->getImage()->threshold( threshold, *image );
	//std::cout << "thresh" << std::endl;
}


SpeckleFilter::SpeckleFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	config->QueryIntAttribute("Noise",&noisecount);
}

void SpeckleFilter::process() {
	input->getImage()->despeckle( *image, noisecount );
	//std::cout << "speckle" << std::endl;
}

