/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <string.h>
#include <iostream>

#include <osc/OscReceivedElements.h>
#include <osc/OscPacketListener.h>
#include <ip/UdpSocket.h>

#include <tisch.h>
#include <BasicBlob.h>
#include <Socket.h>

#define PORT 3333

BasicBlob blob;
UDPSocket* output;

class TUIOtoTISCH : public osc::OscPacketListener {
protected:

	virtual void ProcessMessage( const osc::ReceivedMessage& m, 
			const IpEndpointName& remoteEndpoint )
	{
		try
		{
			if( strcmp( m.AddressPattern(), "/tuio/2Dcur" ) == 0 )
			{
				osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
				const char *msg;
				osc::int32 objectid;
				// osc::int32 classid; //hier evtl. shadow/finger Unterscheidung
				float posx, posy, xspeed, yspeed, maccel;
				args >> msg;

				if (strcmp(msg,"set")==0)
				{
					args >> objectid >> posx >> posy >> xspeed >> yspeed >> maccel;//osc::EndMessage;
					blob.id = objectid + 1000;
					blob.pos.x = posx;
					blob.pos.y = posy;
					blob.peak.x = posx;
					blob.peak.y = posy;
					*output << "shadow " << blob << std::endl;
					blob.id = objectid;
					*output << "finger " << blob << std::endl;
				}
				else if (strcmp(msg,"fseq")==0)
				{
					osc::int32 frameid;
					args >> frameid;
					*output << "frame " << frameid << std::endl;
				}
			}

		} catch( osc::Exception& e ){
			// any parsing errors such as unexpected argument types, or 
			// missing arguments get thrown as exceptions.
			std::cout << "error while parsing message: "
				<< m.AddressPattern() << ": " << e.what() << "\n";
		}
	}
};

int main(int argc, char* argv[])
{

	std::cout << "TUIOtoTISCH - libTISCH 1.1 TUIO adapter" << std::endl;
	std::cout << "(c) 2009 by Andreas Dippon <dippona@in.tum.de>" << std::endl;

	TUIOtoTISCH listener;
	UdpListeningReceiveSocket s(
					IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT ),
					&listener );

	output = new UDPSocket(INADDR_ANY, 0);
	output->target("131.159.10.70", TISCH_PORT_RAW );
	blob.size = 50;
	blob.id = 1;
	blob.pid = 1;
	blob.peak.x = 0;
	blob.peak.y = 0;
	blob.axis1.x = 0; blob.axis1.y = 0;
	blob.axis2.x = 0; blob.axis2.y = 0;

	s.RunUntilSigInt();

	return 0;
}

