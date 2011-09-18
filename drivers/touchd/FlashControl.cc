/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "FlashControl.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <stdexcept>
#include <iostream>
#include <string>


const char* flashmode[] = { "1f", "1", "a", "0" };

#ifndef _MSC_VER

FlashControl::FlashControl( const char* name, int baud ) {

	if ((fd = open( name, O_RDWR | O_NOCTTY )) == -1)
		throw std::runtime_error( std::string("open(") + name + "): " + std::string(strerror(errno)) );

	if (tcgetattr( fd, &current ))
		throw std::runtime_error( std::string("tcgetattr(..): ") + std::string(strerror(errno)) );

	original = current;

	current.c_cflag = baud | CS8 | CLOCAL | CREAD;
	current.c_oflag = 0;
	current.c_iflag = IGNBRK | IGNPAR;
	current.c_lflag = 0;

	cfmakeraw( &current );

	// non-blocking mode for now
	current.c_cc[VTIME] = 0;
	current.c_cc[VMIN]  = 0;

	cfsetospeed( &current, baud );
	cfsetispeed( &current, baud );

	if (tcsetattr( fd, TCSANOW, &current ))
		throw std::runtime_error( std::string("tcsetattr(..): ") + std::string(strerror(errno)) );

}

FlashControl::~FlashControl() {
	set( FLASH_MODE_OFF );
	tcsetattr( fd, TCSANOW, &current );
	close( fd );
}


void FlashControl::set( int cmdnum ) {
	const char* cmd = flashmode[cmdnum];
	write( fd, cmd, strlen(cmd) );
}

#else
	FlashControl::FlashControl( const char* name, int baud ) { }
	FlashControl::~FlashControl() {}
	void FlashControl::set( int cmdnum ) { }
#endif

