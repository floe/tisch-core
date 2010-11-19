/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Filter.h"

#include "V4LImageSource.h"

SourceFilter::SourceFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	int sourcetype;
	//extract source type from config
	config->QueryIntAttribute( "SourceType" , &sourcetype );
	imgsrc = new V4LImageSource("/dev/video0",640,480,30);
	imgsrc->start();
	image = new IntensityImage( 640, 480 );
}

SourceFilter::~SourceFilter() {
	imgsrc->stop();
	delete imgsrc;
}

void SourceFilter::process() {
	imgsrc->acquire();
	imgsrc->getImage( *image );
	imgsrc->release();
}


BGSubFilter::BGSubFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	image = new IntensityImage( 640, 480 );
	background = new ShortImage( 640, 480 );
	config->QueryIntAttribute("Invert",&invert);
}

BGSubFilter::~BGSubFilter() {
	delete background;
}

void BGSubFilter::process() {
	background->subtract( *(input->getImage()), *image, invert );
}


ThreshFilter::ThreshFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	image = new IntensityImage( 640, 480 );
	config->QueryIntAttribute("Threshold",&threshold);
}

ThreshFilter::~ThreshFilter() { }

void ThreshFilter::process() {
	input->getImage()->threshold( threshold, *image );
}

DespeckleFilter::DespeckleFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	image = new IntensityImage( 640, 480 );
	config->QueryIntAttribute("Noise",&noisecount);
}

DespeckleFilter::~DespeckleFilter() { }

void DespeckleFilter::process() {
	input->getImage()->despeckle( *image, noisecount );
}

