/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Settings.h"
#include <string>

template< > std::ostream& operator<< <unsigned char>( std::ostream& s, const _Vector<unsigned char>& t ) {
	return s << (int)t.x << " " << (int)t.y << " " << (int)t.z;
}

template< > std::istream& operator>> <unsigned char>( std::istream& s, _Vector<unsigned char>& t ) {
	int x,y,z;
	s >> x >> y >> z;
	t.set(x,y,z);
	return s;
}


std::ostream& operator<<( std::ostream& s, Settings& set ) {

	s << set.name << "\n";

	s << set.camera.apply    << " ";
	s << set.camera.gain     << " ";
	s << set.camera.exposure << " ";
	s << set.camera.flash    << "\n";

	s << set.vision.threshold << " ";
	s << set.vision.invert    << " ";
	s << set.vision.noise     << " ";
	s << set.vision.bgfactor  << "\n";

	s << set.blob.minsize  << " ";
	s << set.blob.maxsize  << " ";
	s << set.blob.gid      << " ";
	s << set.blob.factor   << " ";
	s << set.blob.radius   << " ";
	s << set.blob.peakdist << " ";
	s << set.blob.cross    << " ";
	s << set.blob.trail    << "\n";

	s << set.video->width       << " ";
	s << set.video->height      << " ";
	s << set.video->fps         << " ";
	s << set.video->source      << " ";
	s << set.video->startgain   << " ";
	s << set.video->startexpo   << " ";
	s << set.video->startbright << " ";
	s << set.video->startflash  << std::endl;

	return s;
}


std::istream& operator>>( std::istream& s, Settings& set ) {

	std::string* tmp = new std::string();
	s >> *tmp; set.name = tmp->c_str();

	s >> set.camera.apply;
	s >> set.camera.gain;
	s >> set.camera.exposure;
	s >> set.camera.flash;

	s >> set.vision.threshold;
	s >> set.vision.invert;
	s >> set.vision.noise;
	s >> set.vision.bgfactor;

	s >> set.blob.minsize;
	s >> set.blob.maxsize;
	s >> set.blob.gid;
	s >> set.blob.factor;
	s >> set.blob.radius;
	s >> set.blob.peakdist;
	s >> set.blob.cross;
	s >> set.blob.trail;

	if (!set.video) set.video = new VideoSettings;

	s >> set.video->width;
	s >> set.video->height;
	s >> set.video->fps;
	s >> set.video->source;
	s >> set.video->startgain;
	s >> set.video->startexpo;
	s >> set.video->startbright;
	s >> set.video->startflash;

	return s;
}

