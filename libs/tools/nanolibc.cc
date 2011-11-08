/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <string.h>
#include <nanolibc.h>

char* optarg = 0;

unsigned int getopt_pos = 1;
unsigned int getopt_num = 1;

int getopt( unsigned int argc, char* argv[], const char* opts ) {
	while (getopt_num < argc) {
		char* curr = argv[getopt_num];
		if (curr[0] != '-') continue;
		while (getopt_pos < strlen(curr)) {
			char opt = curr[getopt_pos++];
			for (unsigned int i = 0; i < strlen(opts); i++) 
				if (opts[i] == opt) 
				{
					if( (i+1 < strlen(opts)) && (opts[i+1] == ':'))
					{
						getopt_num++;
						optarg = (getopt_num < argc ? argv[getopt_num] : 0);
						getopt_num++;
						getopt_pos  = 1;
					}
					return opt;
				}
			return '?';
		}
		getopt_pos  = 1;
		getopt_num += 1;
	}
	return -1;
}

