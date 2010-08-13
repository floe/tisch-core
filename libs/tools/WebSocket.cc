/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdexcept>
#include <string>

#include <cstdio>
#include <string.h>
#include <errno.h>

#include "WebSocket.h"


WebSocket::WebSocket( in_addr_t addr, int port, struct timeval* _timeout ):
	Socket( new WebSocketStream( SOCK_STREAM, addr, port, _timeout ) )
{ }

WebSocket::WebSocket( const char* addr, int port, struct timeval* _timeout ):
	Socket( new WebSocketStream( SOCK_STREAM, inet_convert(addr), port, _timeout ) )
{ }

WebSocket::WebSocket( WebSocketStream* wstr ):
	Socket( wstr )
{ }

WebSocket* WebSocket::listen() {
	WebSocketStream* server = (WebSocketStream*)rdbuf();
	WebSocketStream* stream = (WebSocketStream*)server->listen();
	WebSocket* conn = new WebSocket( stream );

	// read client request
	std::string buf("foo");
	while ((buf != "") && (buf != "\r")) {
		std::getline(*conn,buf);
		std::cout << buf << std::endl;
	}
	std::cout << "done." << std::endl;

	*conn << "HTTP/1.1 101 Web Socket Protocol Handshake\r\n"
           "Upgrade: WebSocket\r\n"
           "Connection: Upgrade\r\n"
					 "WebSocket-Origin: null\r\n"
					 "WebSocket-Location: ws://localhost:12345/websession\r\n\r\n" << std::flush;

	stream->start_filter();
	return conn;
}



WebSocketStream::WebSocketStream( int _type, in_addr_t addr, int port, struct timeval* _timeout ):
	SocketStream( _type, addr, port, _timeout, 0, 0x10000 ), filter( 0 )
{ }

WebSocketStream::WebSocketStream( const WebSocketStream* stream ):
	SocketStream( stream ), filter( 0 )
{ }

WebSocketStream::~WebSocketStream() { }


void WebSocketStream::start_filter() {
	filter = 1;
}

void WebSocketStream::close() {
	// send 0x00 0xFF
	::close( sock );
}


int WebSocketStream::underflow( ) {

	SocketStream::underflow();

	char*  ptr =  gptr();
	char* eptr = egptr() - 1;
	if (!filter) return *ptr;

	if (*ptr  != 0x00) throw std::runtime_error( "Error: WebSocket frame not starting with 0x00!" );
	if (*eptr != 0xFF) throw std::runtime_error( "Error: WebSocket frame not ending with 0xFF!" );

	ptr = ptr+1;
	setg( ptr, ptr, eptr );

	return *ptr;
}

void WebSocketStream::put_char( int chr ) {
	char tmp[3] = { 0x00, chr, 0xFF };
	if (filter)
		sendto( sock,  tmp, 3, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
	else
		sendto( sock, &chr, 1, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
}

void WebSocketStream::put_buffer() {
	char fstart = 0x00;
	char fstop  = 0xFF;
	int len = pptr() - pbase();
	if (len) {
		std::cout << filter << std::endl;
		if (filter) sendto( sock, &fstart, 1, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
		sendto( sock, pbase(), len, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
		if (filter) sendto( sock,  &fstop, 1, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
		setp( pbase(), epptr() );
	}
}

