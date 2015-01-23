/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdexcept>
#include <string>

#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "nanolibc.h"
#include "WebSocket.h"

#include "base64.h"
#include "sha1.h"


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

	// websocket challenge-response
	uint8_t digest[SHA1_HASH_SIZE];
	std::string key, ver;

	// HTTP variables
	std::string request;
	std::string origin;
	std::string host;
	std::string buf;

	// GET/POST request
	*conn >> buf >> request;

	// read client handshake challenge
	while ((buf != "") && (buf != "\r")) {

		std::getline( *conn, buf );

		if (buf.compare(0,23,"Sec-WebSocket-Version: ") == 0) ver = buf.substr(23,buf.length()-24);
		if (buf.compare(0,19,"Sec-WebSocket-Key: "    ) == 0) key = buf.substr(19,buf.length()-20);

		if (buf.compare(0,8,"Origin: ") == 0) origin = buf.substr(8,buf.length()-9);
		if (buf.compare(0,6,"Host: "  ) == 0) host   = buf.substr(6,buf.length()-7);
	}

	key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
	sha1(digest,(uint8_t*)key.c_str(),key.size());

	*conn << "HTTP/1.1 101 Switching Protocols\r\n"
	         "Upgrade: websocket\r\n"
	         "Connection: Upgrade\r\n"
	         //"Sec-WebSocket-Origin: " << origin << "\r\n"
	         //"Sec-WebSocket-Location: ws://" << host << request << "\r\n"
	         //"Sec-WebSocket-Protocol: unknown\r\n\r\n";
					 "Sec-WebSocket-Accept: " << base64( digest, SHA1_HASH_SIZE ) << "\r\n\r\n";

	*conn << std::flush;

	stream->start_filter();
	return conn;
}



WebSocketStream::WebSocketStream( int _type, in_addr_t addr, int port, struct timeval* _timeout ):
	SocketStream( _type, addr, port, _timeout, 0, 0x10000 ), filter( 0 ), raw_end( NULL )
{ }

WebSocketStream::WebSocketStream( const WebSocketStream* stream ):
	SocketStream( stream ), filter( stream->filter )
{ }

WebSocketStream::~WebSocketStream() { }

WebSocketStream* WebSocketStream::clone() { return new WebSocketStream(this); }


void WebSocketStream::start_filter() {
	filter = 1;
}

void WebSocketStream::close() {
	// send 0x00 0xFF
	::close( sock );
}


int WebSocketStream::underflow( ) {

	if (raw_end <= egptr()) {
		SocketStream::underflow();
		raw_end = egptr();
	}

	char*  ptr =  gptr();
	char* eptr = raw_end;
	if (!filter) return *ptr;

	uint8_t* msg = (uint8_t*)ptr;

	// we assume that ptr is now at the start of a new packet header
	int size = msg[1] & 0x7F;
	int mask_off = 2;

	// determine true length & XOR mask offset
	if (size == 126) { size = (msg[2] << 8) | msg[3]; mask_off = 4; }
	else if (size == 127) { /* FIXME - 64 bit length */ mask_off = 10; }

	// save the current XOR mask (possibly still needed for next packet)
	for (int i = 0; i < 4; i++) mask[i] = msg[mask_off+i];

	// the exact amount of payload that is currently available in the buffer
	int rawsize = (eptr-ptr)-(mask_off+4);

	if (rawsize >= size) {
		// we have at least one full packet in the buffer, process it
		for (int i = 0; i < size; i++)
			msg[i+mask_off+4] ^= mask[i%4];
	} else {
		/* FIXME incomplete WS packet, to be continued in next TCP packet */
	}

	ptr = ptr+mask_off+4;
	setg( ptr, ptr, ptr+size );

	return *ptr;
}

void WebSocketStream::put_char( int chr ) {
	uint8_t tmp[3] = { 0x81, 0x01, chr };
	if (filter)
		sendto( sock, tmp,   3, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
	else
		sendto( sock, tmp+2, 1, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
}

void WebSocketStream::put_buffer() {
	int len = pptr() - pbase();
	uint8_t header[4] = { 0x81, len & 0xFF, 0x00, 0x00 };
	int hs = 2;
	if (len > 125) { hs = 4; header[1] = 126; header[2] = (len >> 8)&0xFF; header[3] = len & 0xFF; }
	if (len) {
		if (filter) sendto( sock, &header, hs, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
		sendto( sock, pbase(), len, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
		setp( pbase(), epptr() );
	}
}

