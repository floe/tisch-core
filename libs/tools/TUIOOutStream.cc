/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "TUIOOutStream.h"
#include <time.h>


TUIOOutStream::TUIOOutStream( int _mode, const char* target, int port ):
	osc1( buffer1, TUIOSTREAM_BUFFER_SIZE ),
	osc2( buffer2, TUIOSTREAM_BUFFER_SIZE ),
	transmitSocket( IpEndpointName( target, port ) ),
	frame( 0 ), mode( _mode )
{ }


void TUIOOutStream::start() {

	if (mode & TISCH_TUIO2) {
		// frame message
		osc2 << osc::BeginBundleImmediate;
		osc2 << osc::BeginMessage( "/tuio2/frm" ) << ++frame << osc::TimeTag(time(NULL));
		osc2 << osc::EndMessage;
	}

	if (mode & TISCH_TUIO1) {
		// alive message
		osc1 << osc::BeginBundleImmediate;
		osc1 << osc::BeginMessage( "/tuio/2Dcur" ) << "alive";
		for (std::vector<osc::int32>::iterator id = alive.begin(); id != alive.end(); id++) osc1 << *id;
		osc1 << osc::EndMessage;
	}

	alive.clear();
}


TUIOOutStream& operator<< ( TUIOOutStream& s, const BasicBlob& b ) {

	float w = b.axis1.length();
	float h = b.axis2.length();
	float inner = (b.axis1*(1.0/w))*Vector(1,0,0);
	if (inner >  1.0) inner =  1.0;
	if (inner < -1.0) inner = -1.0;
	float angle = acos(inner);
	if (b.axis1.y < 0) angle = 2*M_PI - angle;

	if (s.mode & TISCH_TUIO2) {
		// /tuio2/bnd s_id x_pos y_pos angle width height area [x_vel y_vel a_vel m_acc r_acc]
		s.osc2 << osc::BeginMessage( "/tuio2/bnd" )
			<< b.id << float(b.pos.x) << float(b.pos.y)
			<< angle << w << h << float(b.size/(w*h))
			<< osc::EndMessage;

		// /tuio2/ptr s_id tu_id c_id x_pos y_pos width press [x_vel y_vel m_acc] 
		s.osc2 << osc::BeginMessage( "/tuio2/ptr" )
			<< b.id << b.type << osc::int32(0)
			<< float(b.peak.x) << float(b.peak.y)
			<< w << float(1.0)
			<< osc::EndMessage;
		
		if (b.pid)
			s.osc2 << osc::BeginMessage( "/tuio2/lia" )
				<< b.pid << true << b.id << osc::int32(0)
				<< osc::EndMessage;
	}

	s.alive.push_back( b.id );

	if (s.mode & TISCH_TUIO1) {
		// /tuio/2Dcur set s x y X Y m
		s.osc1 << osc::BeginMessage( "/tuio/2Dcur" ) << "set"
		  << b.id << float(b.pos.x) << float(b.pos.y)
			<< 0.0f << 0.0f << 0.0f
			<< osc::EndMessage;
		// /tuio/2Dblb set s x y a w h f X Y A m r
		/*s.osc1 << osc::BeginMessage( "/tuio/2Dblb" ) << "set"
		  << b.id << b.pos.x << b.pos.y
			<< angle << w << h << b.size/(w*h)
			<< 0.0 << 0.0 << 0.0 << 0.0 << 0.0
			<< osc::EndMessage;*/
	}

	return s;
}


void TUIOOutStream::send() {

	if (mode & TISCH_TUIO2) {
		// alive message
		osc2 << osc::BeginMessage( "/tuio2/alv" );
		for (std::vector<osc::int32>::iterator id = alive.begin(); id != alive.end(); id++) osc2 << *id;
		osc2 << osc::EndMessage << osc::EndBundle;

		transmitSocket.Send( osc2.Data(), osc2.Size() );
		osc2.Clear();
	}

	if (mode & TISCH_TUIO1) {
		// frame message
		osc1 << osc::BeginMessage( "/tuio/2Dcur" ) << "fseq" << frame << osc::EndMessage;
		osc1 << osc::EndBundle;

		transmitSocket.Send( osc1.Data(), osc1.Size() );
		osc1.Clear();
	}
}

