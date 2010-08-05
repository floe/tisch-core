/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBLIST_H_
#define _BLOBLIST_H_

#include <vector>
#include <iostream>

#include "Settings.h"
#include "GLUTWindow.h"
#include "Blob.h"
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"


class BlobList: public std::vector<Blob> {

	friend std::ostream& operator<<( std::ostream& s, BlobList& l );

	public:

		BlobList( Settings* _settings, IntensityImage* _image = 0 );

		void draw( GLUTWindow* win );
		void track( BlobList* old );

		int  getID( unsigned char value );
		void correlate( BlobList* parents );
		void sendBlobs( osc::OutboundPacketStream& oscOut );

	private:

		Settings* settings;
		IntensityImage* image;

};

std::ostream& operator<<( std::ostream& s, BlobList& l );

#endif // _BLOBLIST_H_

