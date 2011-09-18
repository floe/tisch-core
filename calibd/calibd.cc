/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
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

#include <TUIOInStream.h>
#include <TUIOOutStream.h>


// global objects
Calibration cal;

int run = 1;
int do_calib = 0;

TUIOOutStream* output;
std::ofstream* rawlog = 0;


struct CalibTUIOInput: public TUIOInStream {

	CalibTUIOInput(): TUIOInStream( TISCH_PORT_RAW ) { }
	
	virtual void process_frame() {
		if (rawlog) (*rawlog) << "frame" << std::endl;
		output->send();
		output->start();
	}

	virtual void process_blob( BasicBlob& blob ) {
		if (rawlog) (*rawlog) << blob << std::endl;
		cal.apply( blob );
		*output << blob;
	}

};

CalibTUIOInput input;


int main( int argc, char* argv[] ) {

	char datebuf[1024] = "current";
	const char* address = "127.0.0.1";
	int outport = TISCH_PORT_CALIB;

	#ifndef _MSC_VER
		time_t mytime = time( NULL );
		struct tm* res = localtime( &mytime );
		strftime( datebuf, sizeof(datebuf), "%Y%m%d-%H%M%S", res );
	#endif

	std::ostringstream logname;
	logname << "tisch-rawdata-" << datebuf << ".txt";

	std::cout << "calibd - libTISCH 2.0 calibration layer" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "dlht:p:" ) ) switch (opt) {

		case 'd': if (fork()) return 0; break;
		case 'l': rawlog = new std::ofstream( logname.str().c_str(), std::ios_base::app ); break;

		case 't': address = optarg; break;
		case 'p': outport = atoi(optarg); break;

		case 'h':
		case '?':	std::cout << "Usage: calibd [options]\n";
		          std::cout << "  -t target   use alternate target host (default: 127.0.0.1)\n";
		          std::cout << "  -p udpport  use alternate target port (default: 3333)\n";
		          std::cout << "  -d          fork into background\n";
		          std::cout << "  -l          log all data to " << logname.str() << std::endl;
		          std::cout << "  -h          this\n";
		          return 0; break;
	}

	output = new TUIOOutStream( TISCH_TUIO1 | TISCH_TUIO2, address, outport );
	output->start();

	cal.load();
	input.run();

	std::cout << "Cleaning up.." << std::flush;

	delete rawlog;
	delete output;

	std::cout << "done. Goodbye." << std::endl;

	return 0;
}

