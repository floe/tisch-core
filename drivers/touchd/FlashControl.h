/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _FLASHCONTROL_H_
#define _FLASHCONTROL_H_

#define FLASH_MODE_FINGER 0
#define FLASH_MODE_SHADOW 1
#define FLASH_MODE_AUTO   2
#define FLASH_MODE_OFF    3

#ifndef _MSC_VER
	#include <termios.h>
	#include <unistd.h>
#else
	struct termios {};
	#define B57600 1
#endif

class FlashControl {

	public:

		FlashControl( const char* name, int baud = B57600 );
		~FlashControl();

		void set( int cmd );

	private:

		struct termios original, current;
		int fd;
};

#endif // _FLASHCONTROL_H_

