/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <iostream>
#include <string>
#include <stdlib.h>

#include <nanolibc.h>

#include <signal.h>
#include <math.h>

#include "Calibration.h"
#include "UDPSocket.h"

#include "tisch.h"

// global objects
Calibration cal;
UDPSocket* input;
UDPSocket* output;

int run = 1;
int do_calib = 0;


void handler( int signal ) { run = 0; if (signal == SIGHUP) do_calib = 1; }


int main( int argc, char* argv[] ) {

	std::cout << "calibd - TISCH calibration layer 1.0 beta1" << std::endl;
	std::cout << "(c) 2008 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "dh" ) ) switch (opt) {

		case 'd': if (fork()) return 0; break;

		case 'h':
		case '?':	std::cout << "Usage: calibd [options]\n";
		          std::cout << "  -d  fork into background\n";
		          std::cout << "  -h  this\n";
							std::cout << "  ..  All other options are passed to calibtool on SIGHUP.\n";
		          return 0; break;
	}

	cal.load();

	signal( SIGHUP, handler );
	signal( SIGINT, handler );

	input  = new UDPSocket( INADDR_ANY, TISCH_PORT_RAW );
	output = new UDPSocket( INADDR_ANY, 0 );
	output->target( INADDR_LOOPBACK, TISCH_PORT_CALIB );

	std::string id;
	BasicBlob blob;
	int framenum;

	while (run) {
		(*input) >> id;
		if (id.compare("frame")) {
			(*input) >> blob;
			if (!(*input)) { input->flush(); continue; }
			cal.apply( blob );
			(*output) << id << " " << blob << std::endl;
		} else {
			(*input)  >> framenum;
			if (!(*input)) { input->flush(); continue; }
			(*output) << id << " " << framenum << std::endl;
		}
	}

	std::cout << "Cleaning up.." << std::flush;

	delete input;
	delete output;

	std::cout << "done. Goodbye." << std::endl;

	if (do_calib) { execv( "./calibtool", argv ); }

	return 0;
}

