/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _TUIOSTREAM_H_
#define _TUIOSTREAM_H_

#include <string>
#include <vector>

#include <osc/OscOutboundPacketStream.h>
#include <ip/UdpSocket.h>
#include <BasicBlob.h>
#include <tisch.h>

#define TUIOSTREAM_BUFFER_SIZE 0x10000
#define TISCH_TUIO1 1 // send TUIO 1.0 format
#define TISCH_TUIO2 2 // send TUIO 2.0 format


class TISCH_SHARED TUIOOutStream {

	friend TISCH_SHARED TUIOOutStream& operator<< ( TUIOOutStream& s, const BasicBlob& b );

	public:

		TUIOOutStream( int mode = TISCH_TUIO2, const char* target = "127.0.0.1", int port = TISCH_PORT_CALIB );

		void start();
		void send();

	protected:

		char buffer1[TUIOSTREAM_BUFFER_SIZE];
		char buffer2[TUIOSTREAM_BUFFER_SIZE];
		osc::OutboundPacketStream osc1, osc2;

		UdpTransmitSocket transmitSocket;

		std::vector<osc::int32> alive;
		osc::int32 frame;

		int mode;
};


TISCH_SHARED TUIOOutStream& operator<< ( TUIOOutStream& s, const BasicBlob& b );

#endif // _TUIOSTREAM_H_

