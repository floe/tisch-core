/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BlobList.h"

#include <sstream>


// create new BlobList from a {0,255}-image
BlobList::BlobList( Settings* _settings, IntensityImage* _image ):
	std::vector<Blob>()
{
	settings = _settings;
	image = _image;

	if (!image) return;

	int width  = image->getWidth();
	int height = image->getHeight();

	// frame-local blob counter to differentiate between blobs
	unsigned char value = 254;

	// scan for finger spots
	unsigned char* data = image->getData();
	for (int i = 0; i < width*height; i++) if (data[i] == 255) try {

		// try to create a new blob. throws if blob too small, continues silently.
		push_back( Blob( image, Point(i%width,i/width), value, &(settings->blob) ) );

		// adjust counters
		value--;
		settings->blob.gid++;

		// did the frame-local blob counter overflow?
		if (value == 0) {
			value = 254;
			std::cerr << "Warning: too many " << settings->name << " blobs!" << std::endl;
		}

	} catch (...) { }
}


// update IDs from a previous list
void BlobList::track( BlobList* old ) {

	// for each old blob: find the nearest new blob at the predicted location
	for ( std::vector<Blob>::iterator oldblob = old->begin(); oldblob != old->end(); oldblob++ ) {

		double speed = oldblob->speed.length();
		Vector newpos = oldblob->pos + oldblob->speed;

		double mindist = speed + settings->blob.radius;
		std::vector<Blob>::iterator nearest = end();

		// find closest new blob within search radius (that hasn't yet been assigned)
		for ( std::vector<Blob>::iterator newblob = begin(); newblob != end(); newblob++ ) {
			if (newblob->tracked) continue;
			Vector delta = newblob->pos - newpos;
			double dist = delta.length();
			if (dist < mindist) {
				mindist = dist;
				nearest = newblob;
			}
		}

		// if new blob found, assign previous id/peak/speed and remove from search list
		if ( nearest != end() ) {
			nearest->id = oldblob->id;
			nearest->peak = oldblob->peak;
			nearest->speed = nearest->pos - oldblob->pos;
			nearest->tracked = oldblob->tracked + 1;
		}
	}

	// for each new blob: find peak according to old peak, major and minor axis
	for ( std::vector<Blob>::iterator blob = begin(); blob != end(); blob++ )
		blob->setPeak( image, settings->blob.factor, settings->blob.peakdist );
}


// retrieve a GID using a frame-local value
int BlobList::getID( unsigned char value ) {
	if (value == 0) return 0;
	for ( std::vector<Blob>::iterator blob = begin(); blob != end(); blob++ )
		if (blob->value == value) return blob->id;
	return 0;
}


// find parent IDs from another list
void BlobList::correlate( BlobList* parents ) {
	for ( std::vector<Blob>::iterator blob = begin(); blob != end(); blob++ ) {
		unsigned char value = blob->scan( parents->image, settings->blob.factor );
		int pid = parents->getID( value );
		blob->pid = pid;
	}
}


// draw the entire list to a window, taking care to minimize GL state switches
void BlobList::draw( GLUTWindow* win ) {

	double xoff,yoff,height,size;
	height = win->getHeight();

	glTranslatef(0,0,500); // FIXME: compensate video image default z offset

	// draw trails
	glColor4f( settings->blob.trail.x, settings->blob.trail.y, settings->blob.trail.z, 1.0 );
	glBegin( GL_LINES );

	for ( std::vector<Blob>::iterator blob = begin(); blob != end(); blob++ ) {

		xoff = blob->pos.x;
		yoff = height - blob->pos.y;

		glVertex2d( xoff, yoff );
		glVertex2d( xoff - blob->speed.x, yoff + blob->speed.y );
	}

	glEnd();

	// draw size-adjusted crosshairs
	glColor4f( settings->blob.cross.x, settings->blob.cross.y, settings->blob.cross.z, 1.0 );
	glBegin( GL_LINES );

	for ( std::vector<Blob>::iterator blob = begin(); blob != end(); blob++ ) {

		xoff = blob->pos.x;
		yoff = height - blob->pos.y;
		size = sqrt((double)blob->size)/settings->blob.factor;

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
	for ( std::vector<Blob>::iterator blob = begin(); blob != end(); blob++ ) {
		std::ostringstream tmp;
		tmp << blob->id; if (blob->pid) tmp << "." << blob->pid;
		win->print( tmp.str(), (int)blob->peak.x, (int)blob->peak.y );
	}
}


// dump the list into an ostream, prefixing every entry with the list name
std::ostream& operator<<( std::ostream& s, BlobList& l ) {

	for ( std::vector<Blob>::iterator blob = l.begin(); blob != l.end(); blob++ )
		s << l.settings->name << " " << *blob << std::endl;

	return s;
}

void BlobList::sendBlobs(osc::OutboundPacketStream& oscOut)
{
	//shadow
//	/tuio2/tok s_id tu_id c_id x_pos y_pos angle [x_vel y_vel a_vel m_acc r_acc] 
	if( std::string(settings->name) == "shadow" )
	{
		for( std::vector<Blob>::iterator it = begin(); it != end(); it++)
		{
			oscOut	<< osc::BeginMessage( "/tuio2/tok" )
					<< 0 << 0
					<< it->id
					<< it->pos.x
					<< it->pos.y
					<< 0//TODO angle
					<< osc::EndMessage;

//	/tuio2/_cPPPPPPPP c_id parent_id size peak.x peak.y axis1.x axis1.y axis2.x axis2.y
			oscOut	<< osc::BeginMessage( "/tuio2/_cPPPPPPPP" )
					<< it->id
					<< it->pid
					<< it->size
					<< it->peak.x
					<< it->peak.y
					<< it->axis1.x
					<< it->axis1.y
					<< it->axis2.x
					<< it->axis2.y
					<< osc::EndMessage;
		}
	}

	//finger
//	/tuio2/ptr s_id tu_id c_id x_pos y_pos width press [x_vel y_vel m_acc] 
	else if( std::string(settings->name) == "finger" )
	{
		for( std::vector<Blob>::iterator it = begin(); it != end(); it++)
		{
			oscOut	<< osc::BeginMessage( "/tuio2/ptr" )
					<< 0 << 0
					<< it->id
					<< it->pos.x
					<< it->pos.y
					<< it->axis2.length()
					<< 0
					<< osc::EndMessage;
			
//	/tuio2/_cPPPPPPPP c_id parent_id size peak.x peak.y axis1.x axis1.y axis2.x axis2.y
			oscOut	<< osc::BeginMessage( "/tuio2/_cPPPPPPPP" )	
					<< it->id
					<< it->pid
					<< it->size
					<< it->peak.x
					<< it->peak.y
					<< it->axis1.x
					<< it->axis1.y
					<< it->axis2.x
					<< it->axis2.y
					<< osc::EndMessage;
		}
	}
}
