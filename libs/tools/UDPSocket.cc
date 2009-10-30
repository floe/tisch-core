/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdexcept>
#include <string>

#include <cstdio>
#include <string.h>
#include <errno.h>

#include "UDPSocket.h"


// helper function to get address from ip/hostname string
in_addr_t inet_convert( const char* addr ) {
	struct addrinfo* ai;
	if (getaddrinfo( addr, 0, 0, &ai ) || !ai) return 0;
	struct sockaddr_in* sai = (struct sockaddr_in*)ai->ai_addr;
	in_addr_t result = sai->sin_addr.s_addr;
	freeaddrinfo( ai );
	return ntohl(result);
}


UDPSocket::UDPSocket( in_addr_t addr, int port, struct timeval* _timeout, int _verbose, int _size ) :
	std::iostream( new UDPStream( addr, port, _timeout, _verbose, _size ) )
{ }

UDPSocket::UDPSocket( const char* addr, int port, struct timeval* _timeout, int _verbose, int _size ) :
	std::iostream( new UDPStream( inet_convert(addr), port, _timeout, _verbose, _size ) )
{ }


UDPSocket::~UDPSocket() {
	delete rdbuf();
}


void UDPSocket::target( in_addr_t addr, int port ) {
	UDPStream* udpstream = (UDPStream*)rdbuf();
	udpstream->target( addr, port );
}

void UDPSocket::target( const char* addr, int port ) {
	UDPStream* udpstream = (UDPStream*)rdbuf();
	udpstream->target( inet_convert(addr), port );
}

void UDPSocket::target( std::string& addr, int port ) {
	UDPStream* udpstream = (UDPStream*)rdbuf();
	udpstream->target( inet_convert(addr.c_str()), port );
}

in_addr_t UDPSocket::source( int* port ) {
	UDPStream* udpstream = (UDPStream*)rdbuf();
	return udpstream->source( port );
}

in_addr_t UDPSocket::address( int* port ) {
	UDPStream* udpstream = (UDPStream*)rdbuf();
	return udpstream->address( port );
}


void UDPSocket::flush() {
	std::iostream::flush();
	clear();
	ignore( rdbuf()->in_avail() );
}


UDPStream::UDPStream( in_addr_t addr, int port, struct timeval* _timeout, int _verbose, int _size ) :
	std::streambuf(),
	timeout(_timeout),
	udpsock(-1),
	verbose(_verbose),
	size(_size)
{
	#ifdef _MSC_VER
		WSADATA wsaData;
		int res = WSAStartup( MAKEWORD(2,2), &wsaData );
		if (res != NO_ERROR) std::cerr << "Warning: WSAStartup() returned error " << res << std::endl;
	#endif

	socklen_t len = sizeof(local_addr);

	local_addr.sin_family      = AF_INET;
	local_addr.sin_port        = htons( port );
	local_addr.sin_addr.s_addr = htonl( addr );

	if ((udpsock = socket( PF_INET, SOCK_DGRAM, 0 )) == -1) 
		throw std::runtime_error( std::string( "socket: " ) + std::string(strerror(errno)) );

	if (bind( udpsock, (struct sockaddr*)&local_addr, sizeof(local_addr) ) == -1) 
		throw std::runtime_error( std::string( "bind: " ) + std::string(strerror(errno)) );

	if (getsockname( udpsock, (struct sockaddr*)&local_addr, &len ) == -1)
		throw std::runtime_error( std::string( "getsockname: " ) + std::string(strerror(errno)) );

	char* iptr = new char[_size];
	char* optr = new char[_size];

	setp( optr, optr + _size );
	setg( iptr, iptr + _size, iptr + _size );
}

UDPStream::~UDPStream() {
	sync();
	close( udpsock );
	delete[] eback();
	delete[] pbase();
}



// address management

void UDPStream::target( in_addr_t addr, int port ) {
	target_addr.sin_family      = AF_INET;
	target_addr.sin_port        = htons( port );
	target_addr.sin_addr.s_addr = htonl( addr );
}

in_addr_t UDPStream::source( int* port ) {
	if (port) (*port) = ntohs( source_addr.sin_port );
	return ntohl( source_addr.sin_addr.s_addr );
}

in_addr_t UDPStream::address( int* port ) {
	if (port) (*port) = ntohs( local_addr.sin_port );
	return ntohl( local_addr.sin_addr.s_addr );
}


// input functions

int UDPStream::underflow( ) {

	char* ptr = eback();

	// are we "at buffer's end"?
	if (gptr() >= egptr()) {

		struct timeval tvtmp, *tvptr = 0;
		if (timeout) { tvtmp = *timeout; tvptr = &tvtmp; }

		FD_ZERO( &readset ); FD_SET( udpsock, &readset );
		int res = select( udpsock+1, &readset, 0, 0, tvptr );

		if (res ==  0) return EOF;

		if (res == -1) {
			if (errno == EINTR) return EOF;
			throw std::runtime_error( std::string( "select: ") + std::string(strerror(errno)) );
		}

		socklen_t len = sizeof(source_addr);

		/* // probably unnecessary: peek at waiting packet and increase buffer size
		int res = recvfrom( udpsock, ptr, size, MSG_PEEK | MSG_TRUNC, (struct sockaddr*)&source_addr, &len );
		if (res > size) { delete[] ptr; size = 2*res; ptr = new char[size]; }*/

		res = recvfrom( udpsock, ptr, size, 0, (struct sockaddr*)&source_addr, &len );
		if (res == -1) throw std::runtime_error( std::string( "recvfrom: ") + std::string(strerror(errno)) );

		if (verbose) write( 1, ptr, res );
		setg( ptr, ptr, ptr+res );
	}

	return *ptr;
}


// output functions

int UDPStream::overflow( int chr ) {

	put_buffer();
	
	if (chr == EOF) return 0;

	if (pbase() == epptr()) put_char( chr );
	else sputc( chr );

	return 0;
}

int UDPStream::sync() {
	put_buffer();
	return 0;
}

void UDPStream::put_char( int chr ) {
	char tmp = chr;
	sendto( udpsock, &tmp, 1, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
	if (verbose) write( 1, &tmp, 1 );
}

void UDPStream::put_buffer() {
	int len = pptr() - pbase();
	if (len) {
		sendto( udpsock, pbase(), len, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
		if (verbose) write( 1, pbase(), len );
		setp( pbase(), epptr() );
	}
}

