/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBLIST_H_
#define _BLOBLIST_H_

#include <vector>
#include <iostream>

#include <GLUTWindow.h>
#include <TUIOStream.h>

#include "Filter.h"
#include "Blob.h"


class BlobList: public Filter {

	public:

		 BlobList( TiXmlElement* _config, Filter* _input );
		~BlobList();

		virtual void reset();
		virtual int process();

		virtual void draw( GLUTWindow* win );
		virtual void link( Filter* _link   );

		void send( TUIOStream* tuio );

	protected:

		int getID( unsigned char value );

		double radius, factor, peaksize; // tracking & peak settings
		int minsize, maxsize;            // blob detection settings

		// Color cross, trail;
		std::string type;
		int hflip,vflip;

		BlobList* parent;
		int width, height;

		std::vector<Blob>* blobs;
		std::vector<Blob>* oldblobs;
};

#endif // _BLOBLIST_H_

