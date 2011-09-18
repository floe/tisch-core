/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#ifndef _MSC_VER
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/ip.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <netdb.h>
	#define ssocast (void*)
#else
	#include <WinSock2.h>
	#include <ws2tcpip.h>
	#include <io.h>
	typedef int socklen_t;
	typedef unsigned long in_addr_t; 
	#define close closesocket
	#define ssocast (const char*)
#endif

#include <iostream>
#include <sys/types.h>


in_addr_t inet_convert( const char* addr ); 


class SocketStream;

class TISCH_SHARED Socket: public std::iostream {

	public:

		 Socket( int type, in_addr_t addr, int port, struct timeval* _timeout = 0, int _verbose = 0, int _size = 0x10000 );
		 Socket( SocketStream* stream );

		~Socket();

		void target(    in_addr_t addr, int port );
		void target(  const char* addr, int port );
		void target( std::string& addr, int port );

		in_addr_t source( int* port = 0 );
		in_addr_t address( int* port = 0 );

		void flush();
		void close();
};

class TISCH_SHARED UDPSocket: public Socket {

	public:

		UDPSocket(   in_addr_t addr, int port, struct timeval* _timeout = 0, int _verbose = 0, int _size = 2048 );
		UDPSocket( const char* addr, int port, struct timeval* _timeout = 0, int _verbose = 0, int _size = 2048 );
};

class TISCH_SHARED TCPSocket: public Socket {

	public:

		TCPSocket(   in_addr_t addr, int port, struct timeval* _timeout = 0, int _verbose = 0, int _size = 2048 );
		TCPSocket( const char* addr, int port, struct timeval* _timeout = 0, int _verbose = 0, int _size = 2048 );

		TCPSocket* listen();

	private:

		TCPSocket( SocketStream* stream );
};


class TISCH_SHARED SocketStream: public std::streambuf {

	public:

		SocketStream( int _type, in_addr_t addr, int port, struct timeval* _timeout, int _verbose, int _size );
		SocketStream( const SocketStream* stream );

		virtual ~SocketStream();

		void target( in_addr_t addr, int port );
		void close();

		SocketStream* listen();

		in_addr_t source( int* port = 0 );
		in_addr_t address( int* port = 0 );

	protected:

		virtual SocketStream* clone();

		virtual int underflow( );

		int overflow( int chr );
		int	sync();

		virtual void put_buffer();
		virtual void put_char( int chr );

		struct sockaddr_in target_addr;
		struct sockaddr_in source_addr;
		struct sockaddr_in local_addr;

		struct timeval* timeout;
		fd_set readset;

		int sock, verbose, size, type;
};

#endif // _SOCKET_H_

