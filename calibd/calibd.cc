/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>

#include <nanolibc.h>

#include <signal.h>
#include <math.h>

#include "Calibration.h"
#include "Socket.h"

#include "tisch.h"

// global objects
Calibration cal;
UDPSocket* input;
UDPSocket* output;

int run = 1;
int do_calib = 0;


void handler( int signal ) { run = 0; if (signal == SIGUSR1) do_calib = 1; }


int main( int argc, char* argv[] ) {

	char datebuf[1024] = "current";

	#ifndef _MSC_VER
		time_t mytime = time( NULL );
		struct tm* res = localtime( &mytime );
		strftime( datebuf, sizeof(datebuf), "%Y%m%d-%H%M%S", res );
	#endif

	std::ostringstream logname;
	logname << "finger-rawdata-" << datebuf << ".txt";
	std::ofstream* log = 0;

	std::cout << "calibd - libTISCH 1.1 calibration layer" << std::endl;
	std::cout << "(c) 2008 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "dlh" ) ) switch (opt) {

		case 'd': if (fork()) return 0; break;
		case 'l': log = new std::ofstream( logname.str().c_str(), std::ios_base::app ); break;

		case 'h':
		case '?':	std::cout << "Usage: calibd [options]\n";
		          std::cout << "  -d  fork into background\n";
		          std::cout << "  -l  log all data to " << logname.str() << std::endl;
		          std::cout << "  -h  this\n";
							std::cout << "  ..  All other options are passed to calibtool on SIGHUP.\n";
		          return 0; break;
	}

	cal.load();

	signal( SIGHUP,  handler );
	signal( SIGINT,  handler );
	signal( SIGUSR1, handler );

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
			if (log) (*log) << id << " " << blob << std::endl;
		} else {
			(*input)  >> framenum;
			if (!(*input)) { input->flush(); continue; }
			(*output) << id << " " << framenum << std::endl;
			if (log) (*log) << id << " " << framenum << std::endl;
		}
	}

	std::cout << "Cleaning up.." << std::flush;

	delete log;
	delete input;
	delete output;

	std::cout << "done. Goodbye." << std::endl;

	if (do_calib) {
		std::cout << "Starting calibration..." << std::endl;
		execv( "./calibtool", argv );
	}

	return 0;
}

