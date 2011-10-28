/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <signal.h>
#include <algorithm>

#include <nanolibc.h>
#include <Socket.h>
#include <tisch.h>

#include "Matcher.h"

// 9 ms timeout + 1 ms delay ~ 100 Hz
struct timeval tv = { 0, 9000 };

int verbose = 0;

TCPSocket  gstsrc( (in_addr_t)INADDR_ANY, TISCH_PORT_EVENT, &tv );
TCPSocket* gstcon = 0;


struct DaemonMatcher: public Matcher {

	DaemonMatcher(): Matcher() { }

	void request_update( unsigned long long id ) {
		if (gstcon) *gstcon << "update " << id << std::endl;
	}

	void trigger_gesture( unsigned long long id, Gesture* g ) {
		if (gstcon) *gstcon << "gesture " << id << " " << *g << std::endl;
	}
};

DaemonMatcher matcher;


struct GestureThread: public Thread {

	GestureThread(): Thread() { }

	int process( std::istream& src ) {

		std::string cmd;
		unsigned long long id = 0;

		src >> cmd;
		src >> id;

		if ((cmd == "") && (id == 0)) {
			return 1;
		}

		if (!src) {
			if (verbose) std::cout << "error: cmd ='" << cmd << "', id = '" << id << "'" << std::endl;
			return 0;
		}

		// wipe everything when the client quits
		if (cmd == "bye") {
			if (verbose) std::cout << "client signoff - flushing all regions." << std::endl;
			matcher.clear();
			return -1;
		}

		// use blob peak instead of centroid
		if (cmd == "use_peak") {
			matcher.peakmode( 1 );
			return 1;
		}

		// raise/lower the region to the top/bottom of the stack
		if (cmd == "raise") { matcher.raise( id ); return 1; }
		if (cmd == "lower") { matcher.lower( id ); return 1; }

		if (cmd != "region") {
			std::cout << "warning: unknown command '" << cmd << "'" << std::endl; 
			return 0;
		}

		// parse the region descriptor
		Region reg;
		src >> reg;

		// delete region & exit when error or empty
		if ((!src) || (reg.size() == 0)) {
			matcher.remove( id );			
			if (!src) return 0; else return 1;
		}

		matcher.update( id, &reg );

		if (verbose > 1) std::cout << "got region " << id << " " << reg << std::endl;

		return 1;
	}

	void* run() {

		while (1) {

			if (!gstcon) { 
				gstcon = gstsrc.listen();
				if (verbose) std::cout << "client connected." << std::endl;
			}

			int ret = process( *gstcon );

			switch (ret) {
				case  1: gstcon->clear(); break;
				case -1: delete gstcon; gstcon = 0; break;
				case  0: gstcon->flush(); std::cout << "Warning: stream error." << std::endl; break;
			}

			usleep(1000); // necessary to avoid starving the other thread
		}

		return 0;
	}
};

GestureThread gthr;



int main( int argc, char* argv[] ) {

	MatcherTUIOInput input( &matcher );
	int defnum = 0;

	std::cout << "gestured - libTISCH 2.0 interpretation layer" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	for ( int opt = 0; opt != -1; opt = getopt( argc, argv, "vhd:" ) ) switch (opt) {

		case 'v': verbose += 1; break;
		case 'd': defnum = atoi(optarg);
		          std::cout << "selected default gesture set #" << defnum << std::endl;
		          break;

		case 'h':
		case '?': std::cout << "\nUsage: gestured [options]\n\n";
		          std::cout << "  -d num  use default gesture set #num\n";
		          std::cout << "  -v      be verbose\n";
		          std::cout << "  -h      this\n\n";
		          return 0; break;
	}

	matcher.load_defaults( defnum );

	gthr.start();
	input.run();
}

