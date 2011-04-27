/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _INPUTSTATE_H_
#define _INPUTSTATE_H_

#include <string>
#include <vector>
#include <deque>
#include <map>

#include <BasicBlob.h>


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

