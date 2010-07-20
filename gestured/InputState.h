/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _INPUTSTATE_H_
#define _INPUTSTATE_H_

#include <string>
#include <vector>
#include <deque>
#include <map>

#include <BasicBlob.h>

enum InputType {
	INPUT_TYPE_FINGER,
	INPUT_TYPE_HAND,
	INPUT_TYPE_SHADOW,
	INPUT_TYPE_OBJECT,
	INPUT_TYPE_OTHER,
	// always keep as last 
	INPUT_TYPE_COUNT
};

// TODO: this sucks, should be a map<string,int>
extern TISCH_SHARED const char* inputname[INPUT_TYPE_COUNT];


class TISCH_SHARED BlobHistory: public std::deque<BasicBlob> {

	public:

		BlobHistory( int _maxlen = 100 );

		void add( BasicBlob& val );

		Vector avg, stddev, pos;
		BasicBlob first;

		int recent, newblob, total;

	protected:

		int maxlen;
};


class TISCH_SHARED BlobState: public std::map<int,BlobHistory> {

	public:

		BlobState();
		void purge();

		std::vector<int> lost;
		int changed;
};


class TISCH_SHARED InputState: public std::vector<BlobState> {
	public:
		InputState();
		void purge();
		int changed();
};

#endif // _INPUTSTATE_H_

