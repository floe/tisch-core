/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Settings.h"
#include <string>
#include <tinyxml.h>

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

	TiXmlDocument doc;  

	/*TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
	doc.LinkEndChild( decl );*/

	TiXmlElement* root = new TiXmlElement( "TISCHSettings" );  
	doc.LinkEndChild( root );
	root->SetAttribute( "type", set.name );

	TiXmlElement* cam = new TiXmlElement( "CamSettings" );
	root->LinkEndChild( cam );
	cam->SetAttribute( "Apply",    set.camera.apply    );
	cam->SetAttribute( "Gain",     set.camera.gain     );
	cam->SetAttribute( "Exposure", set.camera.exposure );
	cam->SetAttribute( "Flash",    set.camera.flash    );

	TiXmlElement* vis = new TiXmlElement( "VisionSettings" );
	root->LinkEndChild( vis );
	vis->SetAttribute( "Threshold",   set.vision.threshold );
	vis->SetAttribute( "Invert",      set.vision.invert    );
	vis->SetAttribute( "NoiseReduct", set.vision.noise     );
	vis->SetAttribute( "BkGndFactor", set.vision.bgfactor  );

	TiXmlElement* blob = new TiXmlElement( "BlobSettings" );
	root->LinkEndChild( blob );
	blob->SetAttribute( "MinSize",      set.blob.minsize  );
	blob->SetAttribute( "MaxSize",      set.blob.maxsize  );
	blob->SetAttribute( "GlobalID",     set.blob.gid      );
	blob->SetAttribute( "TrackFactor",  set.blob.factor   );
	blob->SetAttribute( "TrackRadius",  set.blob.radius   );
	blob->SetAttribute( "PeakDistance", set.blob.peakdist );
	//blob.SetAttribute( "CrossColor",   set.blob.cross    );
	//blob.SetAttribute( "TrailColor",   set.blob.trail    );

	TiXmlElement* vid = new TiXmlElement( "VideoSettings" );
	root->LinkEndChild( vid );
	vid->SetAttribute( "Width",       set.video->width       );
	vid->SetAttribute( "Height",      set.video->height      );
	vid->SetAttribute( "FPS",         set.video->fps         );
	vid->SetAttribute( "SourceType",  set.video->source      );
	vid->SetAttribute( "StartGain",   set.video->startgain   );
	vid->SetAttribute( "StartExpo",   set.video->startexpo   );
	vid->SetAttribute( "StartBright", set.video->startbright );
	vid->SetAttribute( "StartFlash",  set.video->startflash  );

	TiXmlPrinter printer;
	doc.Accept( &printer );
	s << printer.Str();
	return s;
}


std::istream& operator>>( std::istream& s, Settings& set ) {

	TiXmlDocument doc;
	TiXmlHandle hdoc( &doc );
	s >> doc;

	TiXmlElement* root = hdoc.FirstChildElement().Element();
	TiXmlHandle hroot( root );
	if (!root) return s;

	root->QueryStringAttribute( "type", &set.name );

	TiXmlElement* cam = hroot.FirstChild( "CamSettings" ).Element();
	if (cam) {
		cam->QueryIntAttribute( "Apply",    &set.camera.apply    );
		cam->QueryIntAttribute( "Gain",     &set.camera.gain     );
		cam->QueryIntAttribute( "Exposure", &set.camera.exposure );
		cam->QueryIntAttribute( "Flash",    &set.camera.flash    );
	}

	TiXmlElement* vis = hroot.FirstChild( "VisionSettings" ).Element();
	if (vis) {
		vis->QueryIntAttribute   ( "Threshold",   &set.vision.threshold );
		vis->QueryIntAttribute   ( "Invert",      &set.vision.invert    );
		vis->QueryIntAttribute   ( "NoiseReduct", &set.vision.noise     );
		vis->QueryDoubleAttribute( "BkGndFactor", &set.vision.bgfactor  );
	}

	TiXmlElement* blob = hroot.FirstChild( "BlobSettings" ).Element();
	if (blob) {
		blob->QueryIntAttribute( "MinSize",  &set.blob.minsize  );
		blob->QueryIntAttribute( "MaxSize",  &set.blob.maxsize  );
		blob->QueryIntAttribute( "GlobalID", &set.blob.gid      );
		blob->QueryDoubleAttribute( "TrackFactor",  &set.blob.factor   );
		blob->QueryDoubleAttribute( "TrackRadius",  &set.blob.radius   );
		blob->QueryDoubleAttribute( "PeakDistance", &set.blob.peakdist );
		//blob->QueryIntAttribute( "CrossColor", &set.blob.cross);
		//blob->QueryIntAttribute( "TrailColor", &set.blob.trail);
	}

	TiXmlElement* vid  = hroot.FirstChild( "VideoSettings" ).Element();
	if (vid) {
		if (!set.video) set.video = new VideoSettings;
		vid->QueryIntAttribute( "Width",       &set.video->width       );
		vid->QueryIntAttribute( "Height",      &set.video->height      );
		vid->QueryIntAttribute( "FPS",         &set.video->fps         );
		vid->QueryIntAttribute( "SourceType",  &set.video->source      );
		vid->QueryIntAttribute( "StartGain",   &set.video->startgain   );
		vid->QueryIntAttribute( "StartExpo",   &set.video->startexpo   );
		vid->QueryIntAttribute( "StartBright", &set.video->startbright );
		vid->QueryIntAttribute( "StartFlash",  &set.video->startflash  );
	}

	return s;
}

