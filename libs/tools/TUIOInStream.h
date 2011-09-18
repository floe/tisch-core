/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <map>

#include <osc/OscReceivedElements.h>
#include <osc/OscPacketListener.h>
#include <ip/UdpSocket.h>

#include <tisch.h>
#include <BasicBlob.h>


class TISCH_SHARED TUIOInStream {

	public:

		TUIOInStream( int port = TISCH_PORT_CALIB );

		virtual void process_blob( BasicBlob& b );
		virtual void process_frame();

		void run();
		void stop();

	protected:

		struct TISCH_SHARED ReceiverThread: public osc::OscPacketListener {

			ReceiverThread( TUIOInStream* p );
			
			virtual void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint );

			TUIOInStream* stream;
			std::map<int,BasicBlob> blobs;
		};

	ReceiverThread receiver;
	UdpListeningReceiveSocket sock;

};

