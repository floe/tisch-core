/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Filter.h"


BGSubFilter::BGSubFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	background = new ShortImage( image->getWidth(), image->getHeight() );
	config->QueryIntAttribute( "Invert", &invert );
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
	background->subtract( *(input->getImage()), *image, invert );
	background->update( *(input->getImage()), *(mask->getImage()) );
	result = background->intensity(); // does 'invert' have to be factored in here?
	return 0;
}


ThreshFilter::ThreshFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	threshold = 128;
	config->QueryIntAttribute( "Threshold", &threshold );
}

int ThreshFilter::process() {
	input->getImage()->threshold( threshold, *image );
	return 0;
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
