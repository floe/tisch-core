/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>

#include <nanolibc.h>

#include <signal.h>
#include <math.h>

#include "Calibration.h"
#include "Socket.h"

#include "tisch.h"

#include <osc/OscReceivedElements.h>
#include <osc/OscPacketListener.h>
#include <ip/UdpSocket.h>
#include <osc/OscOutboundPacketStream.h>

// global objects
Calibration cal;
UDPSocket* input;
UDPSocket* output;

using namespace osc;

#define OUTPUT_BUFFER_SIZE 8196
#define ADDRESS "127.0.0.1"

char buffer[OUTPUT_BUFFER_SIZE];
osc::OutboundPacketStream oscOut( buffer, OUTPUT_BUFFER_SIZE );

UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, TISCH_PORT_CALIB ) );

int run = 1;
int do_calib = 0;

BasicBlob blob;


void handler( int signal ) { run = 0; if (signal == SIGUSR1) do_calib = 1; }


struct ReceiverThread : public osc::OscPacketListener 
{
	
virtual void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint )
{
	if( std::string(m.AddressPattern()) == "/tuio2/frm" ) 
	{
		osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
		osc::int32 framenum;
		TimeTag current_time;
		args >> framenum >> current_time;
		
		oscOut  << osc::BeginBundleImmediate;
		oscOut	<< osc::BeginMessage( "/tuio2/frm" )
				<< framenum
				<< current_time
				<< osc::EndMessage;
	}

//	/tuio2/ptr s_id tu_id c_id x_pos y_pos width press [x_vel y_vel m_acc] 
	if( std::string(m.AddressPattern()) == "/tuio2/ptr" ) //finger
	{
		osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
		osc::int32 objectid, unusedid, press;
		double width;
		args >> unusedid >> unusedid >> objectid >> blob.pos.x >> blob.pos.y >> width >> press;
		blob.id = objectid;
}
//	/tuio2/tok s_id tu_id c_id x_pos y_pos angle [x_vel y_vel a_vel m_acc r_acc] 
	else if ( std::string(m.AddressPattern()) == "/tuio2/tok" ) //shadow
	{
std::cout << "calibd:got shadow" << std::endl;
		osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
		osc::int32 objectid, unusedid;
		double angle;
		args >> unusedid >> unusedid >> objectid >> blob.pos.x >> blob.pos.y >> angle;
		blob.id = objectid;
	}
//	/tuio2/_cPPPPPPPP c_id parentid size peak.x peak.y axis1.x axis1.y axis2.x axis2.y
	else if ( std::string(m.AddressPattern()) == "/tuio2/_cPPPPPPPP" )
	{
		osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
		osc::int32 objectid, parentid, size;
		args >> objectid >> parentid >> size >> blob.peak.x >> blob.peak.y >> blob.axis1.x >> blob.axis1.y >> blob.axis2.x >> blob.axis2.y;
		blob.pid = parentid;
		blob.size = size;
		if(objectid != blob.id) return; //packet lost
		cal.apply(blob);
		if(blob.id % 2)//odd equals shadow
		{
			oscOut	<< osc::BeginMessage( "/tuio2/tok" )
					<< 0 << 0
					<< blob.id
					<< blob.pos.x
					<< blob.pos.y
					<< 0//TODO angle
					<< osc::EndMessage;

			//c_id, parentid, size, peak.x, peak.y, axis1.x, axis1.y, axis2.x, axis2.y
			oscOut	<< osc::BeginMessage( "/tuio2/_cPPPPPPPP" )
					<< blob.id
					<< blob.pid
					<< blob.size
					<< blob.peak.x
					<< blob.peak.y
					<< blob.axis1.x
					<< blob.axis1.y
					<< blob.axis2.x
					<< blob.axis2.y
					<< osc::EndMessage;
		}
		else //finger
		{
			oscOut	<< osc::BeginMessage( "/tuio2/ptr" )
					<< 0 << 0
					<< blob.id
					<< blob.pos.x
					<< blob.pos.y
					<< blob.axis2.length()
					<< 0
					<< osc::EndMessage;
			
			//c_id, parentid, size, peak.x, peak.y, axis1.x, axis1.y, axis2.x, axis2.y
			oscOut	<< osc::BeginMessage( "/tuio2/_cPPPPPPPP" )	
					<< blob.id
					<< blob.pid
					<< blob.size
					<< blob.peak.x
					<< blob.peak.y
					<< blob.axis1.x
					<< blob.axis1.y
					<< blob.axis2.x
					<< blob.axis2.y
					<< osc::EndMessage;
		}
	}
	else if( std::string(m.AddressPattern()) == "/tuio2/alv" )
	{
		osc::int32 id;
		std::stringstream ssStream;
		osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
		while(args.Eos())
		{
			args >> id;
			ssStream << " " << id;
		}
		oscOut	<< osc::BeginMessage( "/tuio2/alv" )
				<< ssStream.str().c_str()
				<< osc::EndMessage;
		transmitSocket.Send( oscOut.Data(), oscOut.Size() );
		oscOut.Clear();
	}
}
};

ReceiverThread receiver;


int main( int argc, char* argv[] ) {

	char datebuf[1024] = "current";

	#ifndef _MSC_VER
		time_t mytime = time( NULL );
		struct tm* res = localtime( &mytime );
		strftime( datebuf, sizeof(datebuf), "%Y%m%d-%H%M%S", res );
	#endif

	std::ostringstream logname;
	logname << "finger-rawdata-" << datebuf << ".txt";
	std::ofstream* log = 0;

	std::cout << "calibd - libTISCH 1.1 calibration layer" << std::endl;
	std::cout << "(c) 2008 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "dlh" ) ) switch (opt) {

		case 'd': if (fork()) return 0; break;
		case 'l': log = new std::ofstream( logname.str().c_str(), std::ios_base::app ); break;

		case 'h':
		case '?':	std::cout << "Usage: calibd [options]\n";
		          std::cout << "  -d  fork into background\n";
		          std::cout << "  -l  log all data to " << logname.str() << std::endl;
		          std::cout << "  -h  this\n";
							std::cout << "  ..  All other options are passed to calibtool on SIGHUP.\n";
		          return 0; break;
	}

	cal.load();

	signal( SIGHUP,  handler );
	signal( SIGINT,  handler );
	signal( SIGUSR1, handler );

	UdpListeningReceiveSocket s( IpEndpointName( IpEndpointName::ANY_ADDRESS, TISCH_PORT_RAW ), &receiver );
	s.RunUntilSigInt();

	std::cout << "Cleaning up.." << std::flush;

	delete log;
	delete input;
	delete output;

	std::cout << "done. Goodbye." << std::endl;

	if (do_calib) {
		std::cout << "Starting calibration..." << std::endl;
		execv( "./calibtool", argv );
	}

	return 0;
}

