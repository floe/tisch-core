/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBLIST_H_
#define _BLOBLIST_H_

#include <vector>
#include <iostream>

#include <GLUTWindow.h>
#include <TUIOOutStream.h>

#include "Filter.h"
#include "Blob.h"


class BlobList: public Filter {

	public:

		 BlobList( TiXmlElement* _config, Filter* _input );
		~BlobList();

		virtual void reset();
		virtual int process();

		virtual void draw( GLUTWindow* win, int show_image );
		virtual void link( Filter* _link   );

		void send( TUIOOutStream* tuio );
		const char* name() const { return "BlobList"; }

	protected:

		int getID( unsigned char value );

		BlobList* parent;
		int width, height;

	#ifdef HAS_UBITRACK
		MarkerTracker* mMarkerTracker;
		std::vector<Ubitrack::Vision::SimpleMarkerInfo>* detectedMarkers;
	#endif

		std::vector<Blob>* blobs;
		std::vector<Blob>* oldblobs;
};

#endif // _BLOBLIST_H_

