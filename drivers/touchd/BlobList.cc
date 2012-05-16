/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BlobList.h"

#include <sstream>


// static global ID counter for all TUIO entities
int gid = 0;


// create new BlobList from a {0,255}-image
BlobList::BlobList( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {

	blobs = oldblobs = NULL;
	reset();

	checkImage();
	parent = 0;

	width  = image->getWidth();
	height = image->getHeight();

	type  = 0;
	hflip = 0;
	vflip = 0;

	minsize = 50;
	maxsize = 0;

	factor = 1.5;
	radius = 20;
	peakmode = 0.0;
	ignore_orphans = 0;

	// try to read settings from XML
	config->QueryIntAttribute( "Type",  &type  );
	config->QueryIntAttribute( "HFlip", &hflip );
	config->QueryIntAttribute( "VFlip", &vflip );
	config->QueryIntAttribute( "IgnoreOrphans", &ignore_orphans );

	config->QueryIntAttribute( "MinSize",  &minsize  );
	config->QueryIntAttribute( "MaxSize",  &maxsize  );

	config->QueryDoubleAttribute( "TrackRadius", &radius   );
	config->QueryDoubleAttribute( "PeakFactor",  &factor   );
	config->QueryDoubleAttribute( "PeakMode",    &peakmode );
	//config->QueryIntAttribute( "CrossColor", &cross);
	//config->QueryIntAttribute( "TrailColor", &trail);

#ifdef HAS_FREENECT
	// MarkerTracker
	config->QueryIntAttribute( "MarkerTracker", &int_mt_enabled );
	config->QueryIntAttribute( "MTthresh", &int_mt_thresh );
	config->QueryIntAttribute( "MTbwthresh", &int_mt_bw_thresh );
	config->QueryIntAttribute( "MTmax", &int_mt_max );
	config->QueryIntAttribute( "MTshowMarker", &int_mt_showMarker );

	// check values for value range and set default values
	(int_mt_enabled == 1)								? mt_enabled = true					: mt_enabled = false;
	(int_mt_thresh >= 0 && int_mt_thresh <= 255)		? mt_thresh = int_mt_thresh			: mt_thresh = 100;
	(int_mt_bw_thresh >= 0 && int_mt_bw_thresh <= 255)	? mt_bw_thresh = int_mt_bw_thresh	: mt_bw_thresh = 40;
	(int_mt_max >= 0 && int_mt_max <= 255)				? mt_max = int_mt_max				: mt_max = 255;
	(int_mt_showMarker == 1)							? mt_showMarker = true				: mt_showMarker = false;

	// create MarkerTracker with read settings
	mMarkerTracker = new MarkerTracker(mt_thresh, mt_bw_thresh, mt_max, width, height);

	foundMarkers = new std::vector<MarkerTracker::markerData>;

	// setting variables for Configurator
	countOfOptions = 10; // quantity of variables that can be manipulated
#else
	countOfOptions = 5; // quantity of variables that can be manipulated
#endif
}

BlobList::~BlobList() {
	delete blobs;
	delete oldblobs;
	delete mMarkerTracker;
	delete foundMarkers;
}

void BlobList::reset() {
	delete blobs;
	delete oldblobs;
	blobs = new std::vector<Blob>;
	oldblobs = NULL;
}

void BlobList::link( Filter* _link ) {
	parent = dynamic_cast<BlobList*>(_link);
}


int BlobList::process() {

	// swap blob lists
	delete oldblobs;
	oldblobs = blobs;
	blobs = new std::vector<Blob>;

	// clone the input image
	*image = *(input->getImage());
	if(!useIntensityImage) 
	{
		*shortimage = *(input->getShortImage());
		shortimage->convert(*image);

#ifdef HAS_FREENECT
		rgbimage = input->getRGBImage(); // get pointer from previous filter, do nothing

		if( mt_enabled ) {
			// call MarkerTracker here
			mMarkerTracker->findMarker(rgbimage, image, foundMarkers);

			//cout << "#Marker: " << foundMarkers->size() << endl;

		}
#endif
	}

	// frame-local blob counter to differentiate between blobs
	unsigned char value = 254;

	// scan for bright spots
	unsigned char* data = image->getData();
	for (int i = 0; i < width*height; i++) if (data[i] == 255) try {

		// try to create a new blob. throws if blob too small, continues silently.
		blobs->push_back( Blob( image, Point(i%width,i/width), value, gid, minsize, maxsize ) );

		// adjust counters
		value--;
		gid++;

		// did the frame-local blob counter overflow?
		if (value == 0) {
			value = 254;
			std::cerr << "Warning: too many type " << type << " blobs!" << std::endl;
		}

	} catch (...) { }

	// ---------------------------------------------------------------------------
	// tracking: update IDs from a previous list
	if (!oldblobs) return 0;

	// for each old blob: find the nearest new blob at the predicted location
	for ( std::vector<Blob>::iterator oldblob = oldblobs->begin(); oldblob != oldblobs->end(); oldblob++ ) {

		double speed = oldblob->speed.length();
		Vector newpos = oldblob->pos + oldblob->speed;

		double mindist = speed + radius;
		std::vector<Blob>::iterator nearest = blobs->end();

		// find closest new blob within search radius (that hasn't yet been assigned)
		for ( std::vector<Blob>::iterator newblob = blobs->begin(); newblob != blobs->end(); newblob++ ) {
			if (newblob->tracked) continue;
			Vector delta = newblob->pos - newpos;
			double dist = delta.length();
			if (dist < mindist) {
				mindist = dist;
				nearest = newblob;
			}
		}

		// if new blob found, assign previous id/peak/speed and remove from search list
		if ( nearest != blobs->end() ) {
			nearest->id = oldblob->id;
			nearest->peak = oldblob->peak;
			nearest->speed = nearest->pos - oldblob->pos;
			nearest->tracked = oldblob->tracked + 1;
		}
	}

	// ---------------------------------------------------------------------------
	// for each new blob: find peak according to old peak, major and minor axis
	for ( std::vector<Blob>::iterator blob = blobs->begin(); blob != blobs->end(); blob++ )
		blob->setPeak( image, factor, peakmode );

	// ---------------------------------------------------------------------------
	// TODO: allow disabling this feature (even if parent set)
	// find parent IDs from another list (if available)
	if (!parent) return 0;
	std::vector<Blob>::iterator blob = blobs->begin(); 
	while ( blob != blobs->end() ) {
		unsigned char value = blob->scan( parent->image, factor );
		int pid = parent->getID( value );
		if (ignore_orphans && !pid) blob = blobs->erase( blob );
		else { blob->pid = pid; blob++; }
	}

	return 0;
}


// retrieve a GID using a frame-local value
int BlobList::getID( unsigned char value ) {
	if (value == 0) return 0;
	for ( std::vector<Blob>::iterator blob = blobs->begin(); blob != blobs->end(); blob++ )
		if (blob->value == value) return blob->id;
	return 0;
}


// draw the entire list to a window, taking care to minimize GL state switches
void BlobList::draw( GLUTWindow* win ) {
#ifdef HAS_FREENECT
	if( displayRGBImage ) {
		win->show( *rgbimage, 0, 0 );
	}
	else {
#endif //HAS_FREENECT
		double xoff,yoff,height,size;
		height = win->getHeight();

		win->show( *image, 0, 0 );

		glTranslatef(0,0,500); // FIXME: compensate video image default z offset
		glLineWidth(2.0);

		// draw trails
		// glColor4f( trail.x, trail.y, trail.z, 1.0 );
		glColor4f( 0.0, 0.0, 1.0, 1.0 );
		glBegin( GL_LINES );

		for ( std::vector<Blob>::iterator blob = blobs->begin(); blob != blobs->end(); blob++ ) {

			xoff = blob->pos.x;
			yoff = height - blob->pos.y;

			glVertex2d( xoff, yoff );
			glVertex2d( xoff - blob->speed.x, yoff + blob->speed.y );
		}

		glEnd();

		// draw size-adjusted crosshairs
		//glColor4f( cross.x, cross.y, cross.z, 1.0 );
		glColor4f( 0.0, 1.0, 0.0, 1.0 );
		glBegin( GL_LINES );

		for ( std::vector<Blob>::iterator blob = blobs->begin(); blob != blobs->end(); blob++ ) {

			xoff = blob->pos.x;
			yoff = height - blob->pos.y;
			size = sqrt((double)blob->size)/factor;

			glVertex2d( xoff - blob->axis1.x, yoff + blob->axis1.y );
			glVertex2d( xoff + blob->axis1.x, yoff - blob->axis1.y );

			glVertex2d( xoff - blob->axis2.x, yoff + blob->axis2.y );
			glVertex2d( xoff + blob->axis2.x, yoff - blob->axis2.y );

			/*glVertex2f( xoff - size, yoff );
			glVertex2f( xoff + size, yoff );
			glVertex2f( xoff, yoff - size );
			glVertex2f( xoff, yoff + size );*/
		}

		glEnd();

		// print IDs (and parent IDs)
		for ( std::vector<Blob>::iterator blob = blobs->begin(); blob != blobs->end(); blob++ ) {
			std::ostringstream tmp;
			tmp << blob->id; if (blob->pid) tmp << "." << blob->pid;
			win->print( tmp.str(), (int)blob->peak.x, (int)blob->peak.y );
		}
#ifdef HAS_FREENECT
	}
	
	if( mt_enabled ) {
		glColor4f( 1.0, 0.0, 0.0, 1.0 );
		win->print( std::string("makertracker running"), 10, win->getHeight() - 30 );
	}

	if( mt_enabled && mt_showMarker ) {
		
		glColor4f( 1.0, 0.0, 0.0, 1.0 );

		std::ostringstream MarkerID;
		MarkerID.str("");
		MarkerID << "marker ";

		std::vector<MarkerTracker::markerData>::iterator iter;
		for(iter = foundMarkers->begin(); iter < foundMarkers->end(); iter++) {
			MarkerID << std::hex << setfill('0') << setw(2) << nouppercase << iter->markerID << " ";
		}

		win->print( MarkerID.str(), 10, win->getHeight() - 50  );
		
	}
#endif
}


// send blob list via OSC as TUIO 2.0
void BlobList::send( TUIOOutStream* oscOut ) {

	for (std::vector<Blob>::iterator it = blobs->begin(); it != blobs->end(); it++) {

		BasicBlob tmp = *it;
		tmp.type = type;

		tmp.pos.x  = tmp.pos.x  / (double)width; tmp.pos.y  = tmp.pos.y  / (double)height;
		tmp.peak.x = tmp.peak.x / (double)width; tmp.peak.y = tmp.peak.y / (double)height;

		if (hflip) {
			tmp.pos.x  = 1.0 - tmp.pos.x;
			tmp.peak.x = 1.0 - tmp.peak.x;
		}

		if (vflip) {
			tmp.pos.y  = 1.0 - tmp.pos.y;
			tmp.peak.y = 1.0 - tmp.peak.y;
		}

		*oscOut << tmp;
	}
}

const char* BlobList::getOptionName(int option) {
	const char* OptionName = "";

	switch(option) {
	case 0:
		OptionName = "Horizontal Flip";
		break;
	case 1:
		OptionName = "Vertical Flip";
		break;
	case 2:
		OptionName = "Minimum Size";
		break;
	case 3:
		OptionName = "Maximum Size";
		break;
	case 4:
		OptionName = "Peakmode";
		break;
#ifdef HAS_FREENECT
	// MarkerTracker
	case 5:
		OptionName = "MT enabled";
		break;
	case 6:
		OptionName = "MT show marker";
		break;
	case 7:
		OptionName = "MT thresh";
		break;
	case 8:
		OptionName = "MT BW thresh";
		break;
	case 9:
		OptionName = "MT max";
		break;
#endif
	default:
		// leave OptionName empty
		break;
	}

	return OptionName;
}

double BlobList::getOptionValue(int option) {
	double OptionValue = -1.0;

	switch(option) {
	case 0:
		OptionValue = hflip;
		break;
	case 1:
		OptionValue = vflip;
		break;
	case 2:
		OptionValue = minsize;
		break;
	case 3:
		OptionValue = maxsize;
		break;
	case 4:
		OptionValue = peakmode;
		break;
#ifdef HAS_FREENECT
	// MarkerTracker
	case 5:
		OptionValue = mt_enabled;
		break;
	case 6:
		OptionValue = mt_showMarker;
		break;
	case 7:
		OptionValue = mt_thresh;
		break;
	case 8:
		OptionValue = mt_bw_thresh;
		break;
	case 9:
		OptionValue = mt_max;
		break;
#endif
	default:
		// leave OptionValue = -1.0
		break;
	}

	return OptionValue;
}

void BlobList::modifyOptionValue(double delta, bool overwrite) {
	switch(toggle) {
	case 0: // hflip is a boolean value
		if(overwrite) {
			hflip = (delta == 0 ? 0 : (delta == 1 ? 1 : hflip));
		} else {
			hflip += delta;
			hflip = (hflip < 0) ? 0 : (hflip > 1) ? 1 : hflip;
		}
		break;
	case 1: // vflip is a boolean value
		if(overwrite) {
			vflip = (delta == 0 ? 0 : (delta == 1 ? 1 : vflip));
		} else {
			vflip += delta;
			vflip = (vflip < 0) ? 0 : (vflip > 1) ? 1 : vflip;
		}
		break;
	case 2:
		if(overwrite) {
			minsize = (delta < 0) ? 0 : (delta > MAX_VALUE) ? MAX_VALUE : delta;
		} else {
			minsize += delta;
			minsize = (minsize < 0) ? 0 : (minsize > MAX_VALUE) ? MAX_VALUE : minsize;
		}
		break;
	case 3:
		if(overwrite) {
			maxsize = (delta < 0) ? 0 : (delta > MAX_VALUE) ? MAX_VALUE : delta;
		} else {
			maxsize += delta;
			maxsize = (maxsize < 0) ? 0 : (maxsize > MAX_VALUE) ? MAX_VALUE : maxsize;
		}
		break;
	case 4:
		if(overwrite) {
			peakmode = (delta < -MAX_VALUE) ? -MAX_VALUE : (delta > MAX_VALUE) ? MAX_VALUE : delta;
		} else {
			peakmode += delta;
			peakmode = (peakmode < -MAX_VALUE) ? -MAX_VALUE : (peakmode > MAX_VALUE) ? MAX_VALUE : peakmode;
		}
		break;
#ifdef HAS_FREENECT
	// MarkerTracker
	case 5: // mt_enabled is a boolean value
		if(overwrite) {
			mt_enabled = (delta == 0 ? 0 : (delta == 1 ? 1 : mt_enabled));
		} else {
			mt_enabled += delta;
			mt_enabled = (mt_enabled < 0) ? 0 : (mt_enabled > 1) ? 1 : mt_enabled;
		}
		break;
	case 6: // mt_showMarker is a boolean value
		if(overwrite) {
			mt_showMarker = (delta == 0 ? 0 : (delta == 1 ? 1 : mt_showMarker));
		} else {
			mt_showMarker += delta;
			mt_showMarker = (mt_showMarker < 0) ? 0 : (mt_showMarker > 1) ? 1 : mt_showMarker;
		}
		break;
	case 7:
		if(overwrite) {
			mt_thresh = (delta < 0) ? 0 : (delta > mt_max) ? mt_max : delta;
		} else {
			mt_thresh += delta;
			mt_thresh = (mt_thresh < 0) ? 0 : (mt_thresh > mt_max) ? mt_max : mt_thresh;
		}
		// pass new setting to MarkerTracker
		mMarkerTracker->update_thresh(mt_thresh);
		break;
	case 8:
		if(overwrite) {
			mt_bw_thresh = (delta < 0) ? 0 : (delta > mt_max) ? mt_max : delta;
		} else {
			mt_bw_thresh += delta;
			mt_bw_thresh = (mt_bw_thresh < 0) ? 0 : (mt_bw_thresh > mt_max) ? mt_max : mt_bw_thresh;
		}
		// pass new setting to MarkerTracker
		mMarkerTracker->update_bw_thresh(mt_bw_thresh);
		break;
	case 9:
		if(overwrite) {
			mt_max = (delta < 0) ? 0 : (delta > 255) ? 255 : delta;
		} else {
			mt_max += delta;
			mt_max = (mt_max < 0) ? 0 : (mt_max > 255) ? 255 : mt_max;
		}
		break;
	}
#endif
}

TiXmlElement* BlobList::getXMLRepresentation() {
	TiXmlElement* XMLNode = new TiXmlElement( "BlobFilter" );
	
	XMLNode->SetAttribute( "IgnoreOrphans", ignore_orphans );
	XMLNode->SetAttribute( "MinSize",  minsize );
	XMLNode->SetAttribute( "MaxSize",  maxsize );
	XMLNode->SetAttribute( "PeakMode", peakmode );
	XMLNode->SetAttribute( "HFlip", hflip );
	XMLNode->SetAttribute( "VFlip", vflip );
	XMLNode->SetAttribute( "Type",  type  );
	XMLNode->SetAttribute( "TrackRadiusZ", radius );
	XMLNode->SetAttribute( "PeakFactor", factor );

#ifdef HAS_FREENECT
	// MarkerTracker
	XMLNode->SetAttribute( "MarkerTracker", mt_enabled );
	XMLNode->SetAttribute( "MTthresh", mt_thresh );
	XMLNode->SetAttribute( "MTbwthresh", mt_bw_thresh );
	XMLNode->SetAttribute( "MTmax", mt_max );
	XMLNode->SetAttribute( "MTshowMarker", mt_showMarker );
	
#endif

	return XMLNode;
}
