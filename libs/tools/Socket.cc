/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdexcept>
#include <string>

#include <cstdio>
#include <string.h>
#include <errno.h>

#include "Socket.h"


// helper function to get address from ip/hostname string
in_addr_t inet_convert( const char* addr ) {
	struct addrinfo* ai;
	if (getaddrinfo( addr, 0, 0, &ai ) || !ai) return 0;
	struct sockaddr_in* sai = (struct sockaddr_in*)ai->ai_addr;
	in_addr_t result = sai->sin_addr.s_addr;
	freeaddrinfo( ai );
	return ntohl(result);
}


// generic socket class

Socket::Socket( int type, in_addr_t addr, int port, struct timeval* _timeout, int _verbose, int _size ) :
	std::iostream( new SocketStream( type, addr, port, _timeout, _verbose, _size ) )
{ }

Socket::Socket( SocketStream* stream ) :
	std::iostream( stream )
{ }

Socket::~Socket() {
	delete rdbuf();
}


void Socket::target( in_addr_t addr, int port ) {
	SocketStream* stream = (SocketStream*)rdbuf();
	stream->target( addr, port );
}

void Socket::target( const char* addr, int port ) {
	SocketStream* stream = (SocketStream*)rdbuf();
	stream->target( inet_convert(addr), port );
}

void Socket::target( std::string& addr, int port ) {
	SocketStream* stream = (SocketStream*)rdbuf();
	stream->target( inet_convert(addr.c_str()), port );
}

in_addr_t Socket::source( int* port ) {
	SocketStream* stream = (SocketStream*)rdbuf();
	return stream->source( port );
}

in_addr_t Socket::address( int* port ) {
	SocketStream* stream = (SocketStream*)rdbuf();
	return stream->address( port );
}

void Socket::flush() {
	std::iostream::flush();
	clear();
	ignore( rdbuf()->in_avail()-1 );
	ignore( 1 ); // FIXME: workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=42857
}

void Socket::close() {
	flush();
	SocketStream* stream = (SocketStream*)rdbuf();
	stream->close();
}


// UDP specialization

UDPSocket::UDPSocket( in_addr_t addr, int port, struct timeval* _timeout, int _verbose, int _size ) :
	Socket( SOCK_DGRAM, addr, port, _timeout, _verbose, _size )
{ }

UDPSocket::UDPSocket( const char* addr, int port, struct timeval* _timeout, int _verbose, int _size ) :
	Socket( SOCK_DGRAM, inet_convert(addr), port, _timeout, _verbose, _size )
{ }


// TCP specialization

TCPSocket::TCPSocket( in_addr_t addr, int port, struct timeval* _timeout, int _verbose, int _size ) :
	Socket( SOCK_STREAM, addr, port, _timeout, _verbose, _size )
{ }

TCPSocket::TCPSocket( const char* addr, int port, struct timeval* _timeout, int _verbose, int _size ) :
	Socket( SOCK_STREAM, inet_convert(addr), port, _timeout, _verbose, _size )
{ }

TCPSocket::TCPSocket( SocketStream* stream ) :
	Socket( stream )
{ }

TCPSocket* TCPSocket::listen() {
	SocketStream* stream = (SocketStream*)rdbuf();
	return new TCPSocket( stream->listen() );
}


// stream implementation

SocketStream::SocketStream( int _type, in_addr_t addr, int port, struct timeval* _timeout, int _verbose, int _size ) :
	std::streambuf(),
	timeout(_timeout),
	sock(-1),
	verbose(_verbose),
	size(_size),
	type(_type)
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

	if ((sock = socket( PF_INET, type, 0 )) == -1) 
		throw std::runtime_error( std::string( "socket: " ) + std::string(strerror(errno)) );

	if (bind( sock, (struct sockaddr*)&local_addr, sizeof(local_addr) ) == -1) 
		throw std::runtime_error( std::string( "bind: " ) + std::string(strerror(errno)) );

	if (getsockname( sock, (struct sockaddr*)&local_addr, &len ) == -1)
		throw std::runtime_error( std::string( "getsockname: " ) + std::string(strerror(errno)) );

	// set socket options: disable TIME_WAIT and Nagle algorithm
	if (type == SOCK_STREAM) {
		int iopt = 1; struct linger lopt = { 1, 0 };
		setsockopt( sock,  SOL_SOCKET,    SO_LINGER, ssocast(&lopt), sizeof(lopt) );
		setsockopt( sock,  SOL_SOCKET, SO_REUSEADDR, ssocast(&iopt), sizeof(iopt) );
		setsockopt( sock, IPPROTO_TCP,  TCP_NODELAY, ssocast(&iopt), sizeof(iopt) );
	}

	char* iptr = new char[_size];
	char* optr = new char[_size];

	setp( optr, optr + _size );
	setg( iptr, iptr + _size, iptr + _size );
}


SocketStream::SocketStream( const SocketStream* stream ) {

	target_addr = stream->target_addr;
	source_addr = stream->source_addr;
	local_addr  = stream->local_addr;
	timeout     = stream->timeout;
	verbose     = stream->verbose;
	size        = stream->size;

	char* iptr = new char[size];
	char* optr = new char[size];

	setp( optr, optr + size );
	setg( iptr, iptr + size, iptr + size );
}


SocketStream::~SocketStream() {
	sync();
	::close( sock );
	delete[] eback();
	delete[] pbase();
}

SocketStream* SocketStream::clone() { return new SocketStream(this); }


SocketStream* SocketStream::listen() {

	if (::listen( sock, 5 ) == -1) // length of connection queue
		throw std::runtime_error( std::string( "listen: " ) + std::string(strerror(errno)) );

	socklen_t len = sizeof(struct sockaddr);
	int newsock = -1;

	if ((newsock = accept( sock, (struct sockaddr*)&source_addr, &len )) == -1)
		throw std::runtime_error( std::string( "accept: " ) + std::string(strerror(errno)) );

	SocketStream* newstream = clone();
	newstream->sock = newsock;

	return newstream;
}

void SocketStream::close() {
	::close( sock );
}


// address management

void SocketStream::target( in_addr_t addr, int port ) {

	target_addr.sin_family      = AF_INET;
	target_addr.sin_port        = htons( port );
	target_addr.sin_addr.s_addr = htonl( addr );

	if (type == SOCK_STREAM) 
		if (connect( sock, (struct sockaddr*)&target_addr, sizeof(target_addr) ) == -1)
			throw std::runtime_error( std::string( "connect: " ) + std::string(strerror(errno)) );
}

in_addr_t SocketStream::source( int* port ) {
	if (port) (*port) = ntohs( source_addr.sin_port );
	return ntohl( source_addr.sin_addr.s_addr );
}

in_addr_t SocketStream::address( int* port ) {
	if (port) (*port) = ntohs( local_addr.sin_port );
	return ntohl( local_addr.sin_addr.s_addr );
}


// input functions

int SocketStream::underflow( ) {

	char* ptr = eback();

	// are we "at buffer's end"?
	if (gptr() >= egptr()) {

		struct timeval tvtmp, *tvptr = 0;
		if (timeout) { tvtmp = *timeout; tvptr = &tvtmp; }

		FD_ZERO( &readset ); FD_SET( sock, &readset );
		int res = select( sock+1, &readset, 0, 0, tvptr );

		if (res ==  0) return EOF;

		if (res == -1) {
			if (errno == EINTR) return EOF;
			throw std::runtime_error( std::string( "select: ") + std::string(strerror(errno)) );
		}

		socklen_t len = sizeof(source_addr);

		/* // probably unnecessary: peek at waiting packet and increase buffer size
		int res = recvfrom( sock, ptr, size, MSG_PEEK | MSG_TRUNC, (struct sockaddr*)&source_addr, &len );
		if (res > size) { delete[] ptr; size = 2*res; ptr = new char[size]; }*/

		res = recvfrom( sock, ptr, size, 0, (struct sockaddr*)&source_addr, &len );
		if (res == -1) throw std::runtime_error( std::string( "recvfrom: ") + std::string(strerror(errno)) );

		if (verbose) { for (int i = 0; i < res; i++) printf("%02hhx ",ptr[i]); printf("\n"); }
		setg( ptr, ptr, ptr+res );
	}

	return *ptr;
}


// output functions

int SocketStream::overflow( int chr ) {

	put_buffer();
	
	if (chr == EOF) return 0;

	if (pbase() == epptr()) put_char( chr );
	else sputc( chr );

	return 0;
}

int SocketStream::sync() {
	put_buffer();
	return 0;
}

void SocketStream::put_char( int chr ) {
	char tmp = chr;
	sendto( sock, &tmp, 1, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
	if (verbose) printf( "%02hhx\n", tmp );
}

void SocketStream::put_buffer() {
	char* ptr = pbase();
	int len = pptr() - ptr;
	if (len) {
		sendto( sock, ptr, len, 0, (struct sockaddr*)&target_addr, sizeof(target_addr) );
		if (verbose) { for (int i = 0; i < len; i++) printf("%02hhx ",ptr[i]); printf("\n"); }
		setp( ptr, epptr() );
	}
}

