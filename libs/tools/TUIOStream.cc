/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "TUIOStream.h"
#include <time.h>


TUIOStream::TUIOStream( const char* target, int port ):
	oscOut( buffer, TUIOSTREAM_BUFFER_SIZE ), 
	transmitSocket( IpEndpointName( target, port ) ),
	frame( 0 )
{ }


void TUIOStream::start() {

	oscOut << osc::BeginBundleImmediate;

	// frame message
	oscOut << osc::BeginMessage( "/tuio2/frm" ) << frame++ << osc::TimeTag(time(NULL)) << osc::EndMessage;
}


	/* blob/pointer messages
	for (std::vector<Filter*>::iterator filter = mypipe->begin(); filter != mypipe->end(); filter++) {
		BlobList* bl = dynamic_cast<BlobList*>(*filter);
		if (bl) bl->send( oscOut, alive );
	}*/


void TUIOStream::send() {

	// alive message
	oscOut << osc::BeginMessage( "/tuio2/alv" );
	for (std::vector<int>::iterator id = alive.begin(); id != alive.end(); id++) oscOut << *id;
	oscOut << osc::EndMessage;

	oscOut << osc::EndBundle;

	transmitSocket.Send( oscOut.Data(), oscOut.Size() );

	oscOut.Clear();
	alive.clear();
}

