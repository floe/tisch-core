/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef	_UDPSOCKET_H_
#define	_UDPSOCKET_H_

#ifndef _MSC_VER
	#include <sys/socket.h>
	#include <netinet/ip.h>
	#include <arpa/inet.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <netdb.h>
#else
	#include <WinSock2.h>
	#include <ws2tcpip.h>
	#include <io.h>
	typedef int socklen_t;
	typedef unsigned long in_addr_t; 
	#define close closesocket
	#define write _write
#endif

#include <iostream>
#include <sys/types.h>



class TISCH_SHARED UDPSocket: public std::iostream {

	public:

		 UDPSocket(   in_addr_t addr, int port, struct timeval* _timeout = 0, int _verbose = 0, int _size = 2048 );
		 UDPSocket( const char* addr, int port, struct timeval* _timeout = 0, int _verbose = 0, int _size = 2048 );

		~UDPSocket();

		void target(    in_addr_t addr, int port );
		void target(  const char* addr, int port );
		void target( std::string& addr, int port );

		in_addr_t source( int* port = 0 );
		in_addr_t address( int* port = 0 );

		void flush();
};


class TISCH_SHARED UDPStream: public std::streambuf {

	public:

		 UDPStream( in_addr_t addr, int port, struct timeval* _timeout = 0, int _verbose = 0, int _size = 2048 );
		~UDPStream();

		void target( in_addr_t addr, int port );

		in_addr_t source( int* port = 0 );
		in_addr_t address( int* port = 0 );

	protected:

		int underflow( );

		int overflow( int chr );
		int	sync();

	private:

		void put_buffer();
		void put_char( int chr );

		struct sockaddr_in target_addr;
		struct sockaddr_in source_addr;
		struct sockaddr_in local_addr;

		struct timeval* timeout;
		fd_set readset;

		int udpsock, verbose, size;
};

#endif // _UDPSOCKET_H_

