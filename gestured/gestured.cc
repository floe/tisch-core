/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <signal.h>
#include <algorithm>
#include <sstream>

#include <osc/OscReceivedElements.h>
#include <osc/OscPacketListener.h>
#include <ip/UdpSocket.h>

#include <nanolibc.h>
#include <Socket.h>
#include <tisch.h>

#include "Matcher.h"

// 9 ms timeout + 1 ms delay ~ 100 Hz
struct timeval tv = { 0, 9000 };

int verbose = 0;

//UDPSocket blobsrc( INADDR_ANY, TISCH_PORT_CALIB, &tv );
TCPSocket  gstsrc( INADDR_ANY, TISCH_PORT_EVENT, &tv );
TCPSocket* gstcon = 0;

// default gestures as parseable string
const char* defaults = "region 1 0 0 6 \
	move 5 1 Motion 0 31 0 0 0 0 \
	scale 5 1 RelativeAxisScale 0 31 0 0 \
	rotate 5 1 RelativeAxisRotation 0 31 0 0 \
	tap 6 2 BlobID 0 27 0 0 BlobPos 0 27 0 0 0 0 \
	remove 6 1 BlobID 0 31 0 1 -1 \
	release 6 1 BlobCount 0 31 0 2 0 0 \
";

std::map<int,BasicBlob> blobs;


struct DaemonMatcher: public Matcher {

	DaemonMatcher(): Matcher() { }

	void request_update( int id ) {
		if (gstcon) *gstcon << "update " << id << std::endl;
	}

	void trigger_gesture( int id, Gesture* g ) {
		if (gstcon) *gstcon << "gesture " << id << " " << *g << std::endl;
	}
};

DaemonMatcher matcher;


struct GestureThread: public Thread {

	GestureThread(): Thread() { }

	int process( std::istream& src ) {

		std::string cmd;
		unsigned long long id = 0;

		src >> cmd;
		src >> id;

		if ((cmd == "") && (id == 0)) {
			return 1;
		}

		if (!src) {
			if (verbose) std::cout << "error: cmd ='" << cmd << "', id = '" << id << "'" << std::endl;
			return 0;
		}

		// wipe everything when the client quits
		if (cmd == "bye") {
			if (verbose) std::cout << "client signoff - flushing all regions." << std::endl;
			matcher.clear();
			return -1;
		}

		// use blob peak instead of centroid
		if (cmd == "use_peak") {
			// TODO: move to Matcher class
			//use_peak = true;
			return 1;
		}

		// raise/lower the region to the top/bottom of the stack
		if (cmd == "raise") { matcher.raise( id ); return 1; }
		if (cmd == "lower") { matcher.lower( id ); return 1; }

		if (cmd != "region") {
			std::cout << "warning: unknown command '" << cmd << "'" << std::endl; 
			return 0;
		}

		// parse the region descriptor
		Region reg;
		src >> reg;

		// delete region & exit when error or empty
		if ((!src) || (reg.size() == 0)) {
			matcher.remove( id );			
			if (!src) return 0; else return 1;
		}

		matcher.update( id, &reg );

		if (verbose > 1) std::cout << "got region " << id << " " << reg << std::endl;

		return 1;
	}

	void* run() {

		std::istringstream defstream( defaults );
		process( defstream );

		while (1) {

			if (!gstcon) { 
				gstcon = gstsrc.listen();
				if (verbose) std::cout << "client connected." << std::endl;
			}

			int ret = process( *gstcon );

			switch (ret) {
				case  1: gstcon->clear(); break;
				case -1: delete gstcon; gstcon = 0; break;
				case  0: gstcon->flush(); std::cout << "Warning: stream error." << std::endl; break;
			}

			usleep(1000); // necessary to avoid starving the other thread
		}

		return 0;
	}
};

GestureThread gthr;



struct ReceiverThread : public osc::OscPacketListener {
	
	virtual void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint ) {

		osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
		osc::int32 blobid, type, unused, parent;
		double width, height, angle, area;
		bool tmp;

		if (std::string(m.AddressPattern()) == "/tuio2/frm") {

			return;

		} else if (std::string(m.AddressPattern()) == "/tuio2/ptr") {

			// /tuio2/ptr s_id tu_id c_id x_pos y_pos width press [x_vel y_vel m_acc]
			args >> blobid;
			BasicBlob& curblob = blobs[blobid];
			args >> type >> unused >> curblob.peak.x >> curblob.peak.y >> width;
			curblob.id = blobid;
			curblob.type = type ? INPUT_TYPE_FINGER : INPUT_TYPE_SHADOW;

		} else if (std::string(m.AddressPattern()) == "/tuio2/bnd") {

			// /tuio2/bnd s_id x_pos y_pos angle width height area [x_vel y_vel a_vel m_acc r_acc]
			args >> blobid;
			BasicBlob& curblob = blobs[blobid];
			args >> curblob.pos.x >> curblob.pos.y >> angle >> width >> height >> area;
			curblob.id = blobid;
			curblob.axis1 = curblob.axis1 * width;  curblob.axis1.rotate( angle );
			curblob.axis2 = curblob.axis2 * height; curblob.axis2.rotate( angle );
			curblob.size  = width * height * area;

		} else if ( std::string(m.AddressPattern()) == "/tuio2/lia" ) {

			args >> parent >> tmp >> blobid >> unused;
			BasicBlob& curblob = blobs[blobid];
			curblob.pid = parent;

		} else if( std::string(m.AddressPattern()) == "/tuio2/alv" ) {

			for (std::map<int,BasicBlob>::iterator blob = blobs.begin(); blob != blobs.end(); blob++) {
				if (verbose >= 2)
					std::cout << "processing blob: id " << blob->first << " type " << blob->second.type << " geometry " << blob->second << std::endl;
				matcher.process_blob( blob->second );
			}

			blobs.clear();
			matcher.process_gestures();
		}
	}
};


ReceiverThread receiver;

int main( int argc, char* argv[] ) {

	std::cout << "gestured - libTISCH 2.0 interpretation layer" << std::endl;
	std::cout << "(c) 2010 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	for ( int opt = 1; opt < argc; opt++ ) {
		if (std::string(argv[opt]) == "-v") verbose += 1;
	}

	gthr.start();

	UdpListeningReceiveSocket s( IpEndpointName( IpEndpointName::ANY_ADDRESS, TISCH_PORT_CALIB ), &receiver );
	s.RunUntilSigInt();
}

