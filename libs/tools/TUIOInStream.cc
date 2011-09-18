/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "TUIOInStream.h"
#include <string>


TUIOInStream::TUIOInStream( int port ):
	receiver( this ),
	sock( IpEndpointName( IpEndpointName::ANY_ADDRESS, port ), &receiver )
{ }

void TUIOInStream::process_blob( BasicBlob& b ) { }
void TUIOInStream::process_frame() { }

void TUIOInStream::run() { sock.Run(); }
void TUIOInStream::stop() { sock.AsynchronousBreak(); }


TUIOInStream::ReceiverThread::ReceiverThread( TUIOInStream* s ): stream( s ) { }

void TUIOInStream::ReceiverThread::ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint ) {

	osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
	osc::int32 blobid, unused, parent;
	float x, y, width, height, angle, area;
	bool tmp;

	if (std::string(m.AddressPattern()) == "/tuio2/frm") {

		return;

	} else if (std::string(m.AddressPattern()) == "/tuio2/ptr") {

		// /tuio2/ptr s_id tu_id c_id x_pos y_pos width press [x_vel y_vel m_acc]
		args >> blobid;
		BasicBlob& curblob = blobs[blobid];
		args >> curblob.type >> unused >> x >> y >> width;
		curblob.id = blobid;
		curblob.peak.x = x;
		curblob.peak.y = y;

	} else if (std::string(m.AddressPattern()) == "/tuio2/bnd") {

		// /tuio2/bnd s_id x_pos y_pos angle width height area [x_vel y_vel a_vel m_acc r_acc]
		args >> blobid;
		BasicBlob& curblob = blobs[blobid];
		args >> x >> y >> angle >> width >> height >> area;
		curblob.id = blobid;
		curblob.axis1 = curblob.axis1 * width;  curblob.axis1.rotate( angle );
		curblob.axis2 = curblob.axis2 * height; curblob.axis2.rotate( angle );
		curblob.size  = width * height * area;
		curblob.pos.x = x;
		curblob.pos.y = y;

	} else if ( std::string(m.AddressPattern()) == "/tuio2/lia" ) {

		args >> parent >> tmp >> blobid >> unused;
		BasicBlob& curblob = blobs[blobid];
		curblob.pid = parent;

	} else if( std::string(m.AddressPattern()) == "/tuio2/alv" ) {

		for (std::map<int,BasicBlob>::iterator blob = blobs.begin(); blob != blobs.end(); blob++) {
			//if (verbose >= 2)
			//	std::cout << "processing blob: id " << blob->first << " type " << blob->second.type << " geometry " << blob->second << std::endl;
			stream->process_blob( blob->second );
		}

		blobs.clear();
		stream->process_frame();
	}
}
