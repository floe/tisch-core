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


// really minimal MD5 implementation, adapted from Wikipedia article

inline uint32_t leftrotate( uint32_t x, uint32_t c ) {
	return (x << c) | (x >> (32-c));
}

// r specifies the per-round shift amounts
uint32_t r[64] = {
	7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22, 
	5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
	4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
	6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

// use binary integer part of the sines of integers (in rad) as constants
uint32_t k[64] = {
	3614090360UL, 3905402710UL,  606105819UL, 3250441966UL, 4118548399UL, 1200080426UL, 2821735955UL, 4249261313UL,
	1770035416UL, 2336552879UL, 4294925233UL, 2304563134UL, 1804603682UL, 4254626195UL, 2792965006UL, 1236535329UL,
	4129170786UL, 3225465664UL,  643717713UL, 3921069994UL, 3593408605UL,   38016083UL, 3634488961UL, 3889429448UL,
	 568446438UL, 3275163606UL, 4107603335UL, 1163531501UL, 2850285829UL, 4243563512UL, 1735328473UL, 2368359562UL,
	4294588738UL, 2272392833UL, 1839030562UL, 4259657740UL, 2763975236UL, 1272893353UL, 4139469664UL, 3200236656UL,
	 681279174UL, 3936430074UL, 3572445317UL,   76029189UL, 3654602809UL, 3873151461UL,  530742520UL, 3299628645UL,
	4096336452UL, 1126891415UL, 2878612391UL, 4237533241UL, 1700485571UL, 2399980690UL, 4293915773UL, 2240044497UL,
	1873313359UL, 4264355552UL, 2734768916UL, 1309151649UL, 4149444226UL, 3174756917UL,  718787259UL, 3951481745UL,
};

void md5block( unsigned char block[64], unsigned char result[16] ) {

	// note: all variables are unsigned 32 bits and wrap modulo 2^32 when calculating

	// initialize variables
	uint32_t h0 = 0x67452301;
	uint32_t h1 = 0xEFCDAB89;
	uint32_t h2 = 0x98BADCFE;
	uint32_t h3 = 0x10325476;

	// process the message in successive 512-bit chunks

// for each 512-bit chunk of message

	// FIXME - only works for little endian
	uint32_t* w = (uint32_t*)block;

	// initialize hash values for this chunk
	uint32_t a = h0;
	uint32_t b = h1;
	uint32_t c = h2;
	uint32_t d = h3;

	uint32_t f,g,t;

	// main loop over chunk content
	for (int i = 0; i < 64; i++) {

		if (i <= 15) {
			f = (b & c) | ((~b) & d);
			g = i;
		} else if (i <= 31) {
			f = (d & b) | ((~d) & c);
			g = (5*i + 1) % 16;
		} else if (i <= 47) {
			f = b ^ c ^ d;
			g = (3*i + 5) % 16;
		} else {
			f = c ^ (b | (~d));
			g = (7*i) % 16;
		}

		t = d;
		d = c;
		c = b;
		b = b + leftrotate((a + f + k[i] + w[g]) , r[i]);
		a = t;
	}

	// add this chunk's hash to result so far
	h0 = h0 + a;
	h1 = h1 + b;
	h2 = h2 + c;
	h3 = h3 + d;

// end for each 512-bit chunk

	// assemble result - FIXME: only works for little endian
	uint32_t* res = (uint32_t*)result;
	res[0] = h0;
	res[1] = h1;
	res[2] = h2;
	res[3] = h3;
}

void md5sum( unsigned char msg[16], unsigned char result[16] ) {

	unsigned char block[64];

	for (int i = 0; i < 16; i++) block[i] = msg[i];

	// append "1" bit to message
	block[16] = 0x80;

	// append "0" bits until message length in bits = 448 (mod 512)
	for (int i = 17; i < 64; i++) block[i] = 0x00;

	// append bit length of unpadded message as 64-bit little-endian integer to message (= 128)
	block[56] = 0x80;

	md5block( block, result );
}


// handshaking stuff

unsigned int calc_key( std::string key ) {

	unsigned long int spaces = 0;
	std::string nums;

	const char* chr = key.c_str();
	int len = key.length();

	for (int i = 0; i < len; i++) {
		char cur = chr[i];
		if ((cur >= '0') && (cur <= '9')) nums.append( 1, cur );
		if (cur == ' ') spaces++;
	}

	unsigned long long int num = atoll(nums.c_str());
	if ((spaces == 0) || (num % spaces)) throw std::runtime_error( "Error: Sec-WebSocket-Key divisor mismatch. Handshake will fail." );

	/*std::cout << "key: " << key << std::endl;
	std::cout << "nums: " << num << " spaces: " << spaces << " res: " << num/spaces << std::endl; */

	return num / spaces;
}

void get_response( std::string key1_raw, std::string key2_raw, unsigned char challenge[8], unsigned char resp[16] ) {

	unsigned char msg[16];

	uint32_t key1 = calc_key( key1_raw );
	uint32_t key2 = calc_key( key2_raw );

	/*printf("key1: %08x\n",key1);
	printf("key2: %08x\n",key2);*/

	// store keys as big-endian
	msg[0] = (key1 >> 24) & 0xFF; msg[1] = (key1 >> 16) & 0xFF; msg[2] = (key1 >>  8) & 0xFF; msg[3] = (key1 >>  0) & 0xFF;
	msg[4] = (key2 >> 24) & 0xFF; msg[5] = (key2 >> 16) & 0xFF; msg[6] = (key2 >>  8) & 0xFF; msg[7] = (key2 >>  0) & 0xFF;

	for (int i = 0; i < 8; i++) msg[i+8] = challenge[i];

	md5sum( msg, resp );

	/*printf("message: "); for (int i = 0; i < 16; i++) printf("%02x ",msg[i]); printf("\n");
	printf("md5sum:  "); for (int i = 0; i < 16; i++) printf("%02x ",resp[i]); printf("\n");*/
}


// actual socket functions

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
	std::string key1, key2;
	unsigned char challenge[8];
	unsigned char response[16];

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

		if (buf.compare(0,20,"Sec-WebSocket-Key1: ") == 0) key1 = buf.substr(20);
		if (buf.compare(0,20,"Sec-WebSocket-Key2: ") == 0) key2 = buf.substr(20);

		if (buf.compare(0,8,"Origin: ") == 0) origin = buf.substr(8,buf.length()-9);
		if (buf.compare(0,6,"Host: "  ) == 0) host   = buf.substr(6,buf.length()-7);
	}

	conn->read( (char*)challenge, 8 );

	// generate handshake response
	get_response( key1, key2, challenge, response );

	*conn << "HTTP/1.1 101 Web Socket Protocol Handshake\r\n"
	         "Upgrade: WebSocket\r\n"
	         "Connection: Upgrade\r\n"
	         "Sec-WebSocket-Origin: " << origin << "\r\n"
	         "Sec-WebSocket-Location: ws://" << host << request << "\r\n"
	         "Sec-WebSocket-Protocol: unknown\r\n\r\n";

	conn->write( (const char*)response, 16 );
	*conn << std::flush;

	stream->start_filter();
	return conn;
}



WebSocketStream::WebSocketStream( int _type, in_addr_t addr, int port, struct timeval* _timeout ):
	SocketStream( _type, addr, port, _timeout, 0, 0x10000 ), filter( 0 )
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

	SocketStream::underflow();

	char*  ptr =  gptr();
	char* eptr = egptr() - 1;
	if (!filter) return *ptr;

	//for (char* p = ptr; p <= eptr; p++) printf("%02hhx ",*p); printf("\n"); 

	if ((unsigned char)(*ptr ) != 0x00) throw std::runtime_error( "Error: WebSocket frame not starting with 0x00." );
	if ((unsigned char)(*eptr) != 0xFF) throw std::runtime_error( "Error: WebSocket frame not ending with 0xFF." );

	ptr = ptr+1;
	setg( ptr, ptr, eptr );

	return *ptr;
}

void WebSocketStream::put_char( int chr ) {
	char tmp[3] = { 0x00, (char)chr, (char)0xFF };
	if (filter)
		sendto( sock,   tmp,     3, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
	else
		sendto( sock, &(tmp[1]), 1, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
}

void WebSocketStream::put_buffer() {
	char fstart = 0x00;
	char fstop  = 0xFF;
	int len = pptr() - pbase();
	if (len) {
		if (filter) sendto( sock, &fstart, 1, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
		sendto( sock, pbase(), len, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
		if (filter) sendto( sock,  &fstop, 1, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
		setp( pbase(), epptr() );
	}
}

