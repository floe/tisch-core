/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBLIST_H_
#define _BLOBLIST_H_

#include <vector>
#include <iostream>

#include "Filter.h"
#include "GLUTWindow.h"
#include "Blob.h"
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"



class BlobList: public Filter {

	friend std::ostream& operator<<( std::ostream& s, BlobList& l );

	public:

		 BlobList( TiXmlElement* _config, Filter* _input );
		~BlobList();

		virtual void reset();
		virtual void process();
		virtual void link( Filter* _link );

		void draw( GLUTWindow* win );

		int  getID( unsigned char value );
		void correlate( );
		void sendBlobs( osc::OutboundPacketStream& oscOut );

	private:

		BlobSettings settings;
		BlobList* parent;

		std::vector<Blob>* blobs;
		std::vector<Blob>* oldblobs;
};

std::ostream& operator<<( std::ostream& s, BlobList& l );

#endif // _BLOBLIST_H_

