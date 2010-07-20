/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <string.h>
#include <nanolibc.h>

unsigned int getopt_pos = 1;
unsigned int getopt_num = 1;

int getopt( unsigned int argc, char* argv[], const char* opts ) {
	while (getopt_num < argc) {
		char* curr = argv[getopt_num];
		if (curr[0] != '-') continue;
		while (getopt_pos < strlen(curr)) {
			char opt = curr[getopt_pos++];
			for (unsigned int i = 1; i < strlen(opts); i++) if (opts[i] == opt) return opt;
			return '?';
		}
		getopt_pos  = 1;
		getopt_num += 1;
	}
	return -1;
}

