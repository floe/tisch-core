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

		virtual void draw( GLUTWindow* win );
		virtual void link( Filter* _link   );

		void send( TUIOOutStream* tuio );

		// Configurator
		virtual const char* getOptionName(int option);
		virtual double getOptionValue(int option);
		virtual void modifyOptionValue(double delta, bool overwrite);
		virtual TiXmlElement* getXMLRepresentation();

	protected:

		int getID( unsigned char value );

		double radius, factor, peakmode; // tracking & peak settings
		int minsize, maxsize;            // blob detection settings

		// Color cross, trail;
		int type, hflip, vflip;
		int ignore_orphans;

		BlobList* parent;
		int width, height;

		std::vector<Blob>* blobs;
		std::vector<Blob>* oldblobs;
};

#endif // _BLOBLIST_H_

