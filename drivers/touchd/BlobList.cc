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
BlobList::BlobList( TiXmlElement* _config, Filter* _input ):
	Filter( _config, _input, FILTER_TYPE_NONE )
{

	blobs = oldblobs = NULL;
	reset();

	parent = 0;

	if (input->getImage()) {
		width  = input->getImage()->getWidth();
		height = input->getImage()->getHeight();
	} else {
		width  = input->getShortImage()->getWidth();
		height = input->getShortImage()->getHeight();
	}

	image = new IntensityImage( width, height );
	shortimage = NULL;

	if (input->getRGBImage())
		rgbimage = new RGBImage(
			input->getRGBImage()->getWidth(),
			input->getRGBImage()->getHeight()
		);

	// try to read settings from XML
	createOption( "Type",  0, 0, 32 );
	createOption( "HFlip", 0, 0,  1 );
	createOption( "VFlip", 0, 0,  1 );
	createOption( "IgnoreOrphans", 0, 0, 1 );

	createOption( "MinSize", 50, 1 );
	createOption( "MaxSize",  0, 0 );

	createOption( "TrackRadius",  20,  1 );
	createOption( "PeakFactor",  1.5,  1 );
	createOption( "PeakMode",    0.0, -1 );

	// MarkerTracker
	createOption( "MarkerTracker", 0, 0, 1 );
	createOption( "MTshowMarker",  0, 0, 1 );

#ifdef HAS_UBITRACK
	// create MarkerTracker with read settings
	mMarkerTracker = new MarkerTracker(width, height);
	mMarkerTracker->addMarkerID(0x1228);
	mMarkerTracker->addMarkerID(0x1c44);
	mMarkerTracker->addMarkerID(0x0b44);
	mMarkerTracker->addMarkerID(0x0690);
	mMarkerTracker->addMarkerID(0x005a);
	mMarkerTracker->addMarkerID(0x0272);

	detectedMarkers = new std::vector<Ubitrack::Vision::SimpleMarkerInfo>;
#endif
}

BlobList::~BlobList() {
	delete blobs;
	delete oldblobs;
	#ifdef HAS_UBITRACK
	delete mMarkerTracker;
	delete detectedMarkers;
	#endif
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

	// tracking & peak settings
	double radius = options["TrackRadius"]->get();
	double factor = options["PeakFactor"]->get();
	double peakmode = options["PeakMode"]->get();

	// blob detection settings
	int minsize = options["MinSize"]->get();
	int maxsize = options["MaxSize"]->get();

	int ignore_orphans = options["IgnoreOrphans"]->get();
	int type = options["Type"]->get();

	// swap blob lists
	delete oldblobs;
	oldblobs = blobs;
	blobs = new std::vector<Blob>;

	// clone the input image
	if (input->getImage()) {
		*image = *(input->getImage());
	} else {
		input->getShortImage()->convert(*image);

#ifdef HAS_UBITRACK
		if( options["MarkerTracker"]->get() != 0) {
			// call MarkerTracker here
			mMarkerTracker->findMarker(rgbimage, image, detectedMarkers);
		}
#endif
	}

	// frame-local blob counter to differentiate between blobs
	unsigned char value = 254;

	RGBImage* rgb = input->getRGBImage();

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

		if (rgb) {
			Blob& blob = blobs->back();
			std::vector<Point>& border = blob.border;
			std::vector<Point>::iterator pt = border.begin();
			rgbimage->clear();
			unsigned long long int r = 0,g = 0,b = 0,count = 0;
			while (pt != border.end()) {
				Point p1 = *pt; pt++;
				Point p2 = *pt; pt++;
				int offset = rgbimage->pixelOffset(p1.x,p1.y,TR);
				int target = rgbimage->pixelOffset(p2.x,p2.y,TB);
				unsigned char* src = rgb->getData();
				unsigned char* dst = rgbimage->getData();
				while (offset <= target) {
					dst[offset] = src[offset]; r += src[offset++];
					dst[offset] = src[offset]; g += src[offset++];
					dst[offset] = src[offset]; b += src[offset++];
					count++;
				}
			}
			r = r/count;
			g = g/count;
			b = b/count;
			rgb2hsv(r,g,b,blob.h,blob.s,blob.v);
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
	for ( std::vector<Blob>::iterator blob = blobs->begin(); blob != blobs->end(); blob++ ) {
		blob->setPeak( image, factor, peakmode );
	}

	#ifdef HAS_UBITRACK
	// also try to find a blob for each marker; if no blob is found, a blob is created for the marker position
	for(std::vector<Ubitrack::Vision::SimpleMarkerInfo>::iterator marker_iter = detectedMarkers->begin();
		marker_iter != detectedMarkers->end(); marker_iter++)
	{
		bool blob_found = false;
			
		double marker_x = marker_iter->pos[0];
		double marker_y = marker_iter->pos[1];
		double marker_z = marker_iter->pos[2];
		
		//           120cm
		//        ----------  -
		//       /        /   90
		//      /        /    cm
		//     ----------     -
		// when kinect is 100cm above surface

		// (x/z) / (FieldOfView_x_100cm/2) * width/2
		// (y/z) / (FieldOfView_y_100cm/2) * height/2

		// x -2.02 y -1.38			x 1.99 y -1.44
		//
		//
		// x -2.03 y 1.44			x 1.96 y 1.49

		// => x 0..4 y 0..2.8
		// 640 = 4 => 1 = 160
		// 480 = 2.8 => 1 = 171

 		// invert x coordinates (-), move origin to top left corner (+2), map to image coords (*160)
		double mx = (-((marker_x / marker_z) / 60 * 320) + 2) * 160;
		// move origin to top left corner (+1.4), map to image coords (*171)
		double my = (((marker_y / marker_z) / 45 * 240) + 1.4) * 171;

		for ( std::vector<Blob>::iterator blob = blobs->begin(); blob != blobs->end(); blob++ ) {

			double blob_x = blob->pos.x;
			double blob_y = blob->pos.y;

			//std::cout << "mx " << marker_x << " my " << marker_y << " mz " << marker_z << " calX " << mx << " calY " << my << std::endl;
		
			//std::cout << "blobX " << mx << " blobY " << my << std::endl;

			// Thresholds for assign blob <> marker
			// todo: make them changeable via configurator
			double xThresh = 30.0;
			double yThresh = 30.0;

			if(abs(blob_x - mx) < xThresh && abs(blob_y - my) < yThresh) {
				blob->assignedMarker.markerID = marker_iter->ID;
				blob_found = true;
				break;
			}
		}
		if(!blob_found)
		{
			Blob markerblob(value, gid);
			value--; gid++;

			if (value == 0) {
				value = 254;
				std::cerr << "Warning: too many type " << type << " blobs!" << std::endl;
			}

			markerblob.pos.x = mx; markerblob.pos.y = my;
			markerblob.peak.x = mx;	markerblob.peak.y = my;
			markerblob.size = 2000;
			markerblob.type = INPUT_TYPE_FINGER;
			markerblob.assignedMarker.markerID = marker_iter->ID;
			blobs->push_back(markerblob);
		}

	}
	#endif

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
void BlobList::draw( GLUTWindow* win, int show_image ) {

		Filter::draw( win, show_image );

		double xoff,yoff,height;
		height = win->getHeight();

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
			//size = sqrt((double)blob->size)/factor;

			glVertex2d( xoff - blob->axis1.x, yoff + blob->axis1.y );
			glVertex2d( xoff + blob->axis1.x, yoff - blob->axis1.y );

			glVertex2d( xoff - blob->axis2.x, yoff + blob->axis2.y );
			glVertex2d( xoff + blob->axis2.x, yoff - blob->axis2.y );

			//glVertex2f( xoff - size, yoff );
			//glVertex2f( xoff + size, yoff );
			//glVertex2f( xoff, yoff - size );
			//glVertex2f( xoff, yoff + size );
		}

		glEnd();

		// print IDs (and parent IDs)
		for ( std::vector<Blob>::iterator blob = blobs->begin(); blob != blobs->end(); blob++ ) {
			std::ostringstream tmp;
			tmp << blob->id; if (blob->pid) tmp << "." << blob->pid;
			win->print( tmp.str(), (int)blob->peak.x, (int)blob->peak.y );
		}
#ifdef HAS_UBITRACK
	
	bool mt_enabled = options["MarkerTracker"].get();
	bool mt_showMarker = options["MTShowMarker"].get();

	if( mt_enabled ) {
		glColor4f( 1.0, 0.0, 0.0, 1.0 );
		win->print( std::string("makertracker running"), 10, win->getHeight() - 30 );
	}

	if( mt_enabled && mt_showMarker ) {
		
		glColor4f( 1.0, 0.0, 0.0, 1.0 );

		std::ostringstream Blobstream;
		Blobstream.str("");
		Blobstream << "Blob ";
		for ( std::vector<Blob>::iterator blob = blobs->begin(); blob != blobs->end(); blob++ ) {
			
			Blobstream << "x: " << blob->pos.x << " y: " << blob->pos.y;
			
		}
		win->print( Blobstream.str(), 10, win->getHeight() - 70  );


		std::ostringstream MarkerID;
		MarkerID.str("");
		MarkerID << "marker ";

		glLineWidth(2.0);
		glBegin( GL_LINES );

		for(std::vector<Ubitrack::Vision::SimpleMarkerInfo>::iterator iter = detectedMarkers->begin(); iter != detectedMarkers->end(); iter++) {
			MarkerID << std::hex << setfill('0') << setw(2) << nouppercase << iter->ID << " ";

			
			// x axis - red
			//glColor4f( 1.0, 0.0, 0.0, 1.0 );
			//glVertex2d( x * width, y*height);
			//glVertex2d( x*width + 10, y*height);

			// y axis - green
			//glColor4f( 0.0, 1.0, 0.0, 1.0 );
			//glVertex2d(x, y);
			//glVertex2d(x, y+1);

			// z axis - blue
			//glColor4f( 0.0, 0.0, 1.0, 1.0 );
			//glVertex2d();
		}

		glEnd();

		win->print( MarkerID.str(), 10, win->getHeight() - 50  );
		
	}
#endif
}


// send blob list via OSC as TUIO 2.0
void BlobList::send( TUIOOutStream* oscOut ) {

	int hflip = options["HFlip"]->get();
	int vflip = options["VFlip"]->get();

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
