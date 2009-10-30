/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <algorithm>
using namespace std;

#include "ImageSet.h"

/*
#include <fstream>
void dump_plot( int in[], const char* name ) {
  std::ofstream f0( name, std::ios::trunc );
  f0 << "set term x11 persist\nset grid\nset xrange [0:256]\nset yrange [-5000:5000]\nplot '-' with linespoints\n";
  for (int i = 0; i < 256; i++) f0 << i << " " << in[i] << std::endl;
  f0.close();
}*/

ImageSet::ImageSet( Settings* _settings ) {

	settings = _settings;

	raw    = new IntensityImage( settings->video->width, settings->video->height );
	binary = new IntensityImage( settings->video->width, settings->video->height );
	bkgnd  = new     ShortImage( settings->video->width, settings->video->height );
	subtr  = new IntensityImage( settings->video->width, settings->video->height );
	//mask   = new IntensityImage( "mask.pgm" );
	final  = new IntensityImage( settings->video->width, settings->video->height );
}


ImageSet::~ImageSet() {
	delete raw;
	delete binary;
	delete bkgnd;
	delete subtr;
	//delete mask;
	delete final;
}


IntensityImage* ImageSet::getRaw()   { return raw;   }
IntensityImage* ImageSet::getFinal() { return final; }


void ImageSet::swap( ImageSet* other ) {
	std::swap( raw, other->raw );
	//std::swap( img_intensity, other->img_intensity );
}


void ImageSet::process( ) {

	// finger mode: cur - bg = subtr
	// d.h. wenn int(cur) > int(bg) => thresh++, int(cur) - int(bg) > 0 => thresh++
	// shadow mode: bg - cur = subtr
	// d.h. wenn int(bg) > int(cur) => thresh++, int(bg) - int(cur) > 0 <=> int(cur) - int(bg) < 0

	// take the intensity difference between background and current image.
	// add a fixed threshold value, absolute minimum threshold is 5 to avoid noise
	int thresh = (img_intensity - bg_intensity) * (1 - settings->vision.invert*2);
	thresh += settings->vision.threshold;
	thresh = max( thresh, 5 );

	bkgnd->subtract( *raw, *subtr, settings->vision.invert );
	subtr->threshold( thresh, *(binary) );

	// std::cout << "bgint: " << bg_intensity << " imgint: " << img_intensity << " thresh: " << thresh << std::endl;

	int n = settings->vision.noise;
	binary->despeckle( *final, n );
	bkgnd->update( *raw, *final );

	bg_intensity  = bkgnd->intensity();
	//bg_intensity /= settings->video->width * settings->video->height;
}

std::string ImageSet::draw( GLUTWindow* win, int num ) {

	IntensityImage* source = 0;
	std::string info;

	switch (num) {
		case 1: source =    raw; info =         "raw"; break;
		case 2: source =      0; info =  "background"; break;
		case 3: source =  subtr; info =  "subtracted"; break;
		case 4: source = binary; info = "thresholded"; break;
		case 5: source =  final; info =    "denoised"; break;
	}

	if (num == 2) win->show( *bkgnd, 0, 0 );
	if (source) win->show( *source, 0, 0 );
	return info;
}

void ImageSet::update() {
	*bkgnd = *raw;
	bg_intensity  = bkgnd->intensity();
	//bg_intensity /= settings->video->width * settings->video->height;
}

int ImageSet::analyze() {

	int tmp[256];

	img_intensity  = raw->intensity();
	//img_intensity /= settings->video->width * settings->video->height;

	raw->histogram(tmp);

	for (int i = 255; i >= 0; i--) if (tmp[i] > 5) return i;
	return 0;
}

