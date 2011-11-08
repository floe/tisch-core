/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef	_WEBSOCKET_H_
#define	_WEBSOCKET_H_

#include <Socket.h>


class WebSocketStream;

class TISCH_SHARED WebSocket: public Socket {

	public:

		 WebSocket(   in_addr_t addr, int port, struct timeval* _timeout = 0 );
		 WebSocket( const char* addr, int port, struct timeval* _timeout = 0 );

		WebSocket* listen(); 

	private:

		WebSocket( WebSocketStream* wstr );

};


class TISCH_SHARED WebSocketStream: public SocketStream {

	public:

		WebSocketStream( int _type, in_addr_t addr, int port, struct timeval* _timeout );
		WebSocketStream( const WebSocketStream* stream );

		virtual ~WebSocketStream();

		virtual void close();
		void start_filter();

	protected:

		virtual WebSocketStream* clone();

		virtual int underflow( );

		virtual void put_buffer();
		virtual void put_char( int chr );

		int filter;

};

#endif // _WEBSOCKET_H_

