/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "InputState.h"


InputState::InputState(): std::vector<BlobState>(INPUT_TYPE_COUNT) { }

void InputState::purge() {
	for (std::vector<BlobState>::iterator it = begin(); it != end(); it++)
		it->purge();
}

int InputState::changed() {
	for (std::vector<BlobState>::iterator it = begin(); it != end(); it++)
		if (it->changed) return 1;
	return 0;
}


BlobState::BlobState(): std::map<int,BlobHistory>(), lost() { changed = 0; }

// remove blobs which have not been updated since last purge
void BlobState::purge() {

	changed = 0;
	lost.clear();

	for (std::map<int,BlobHistory>::iterator bh = begin(); bh != end(); ) {

		if (bh->second.recent == 0) {

			//std::cout << "removing history for blob #" << bh->first << std::endl;
			changed = 1;
			lost.push_back( bh->first );
			erase(bh++);

		} else {

			if (bh->second.newblob == 1) changed = 1;
			bh->second.newblob = 0;
			bh->second.recent = 0;
			bh++;

		}
	}
}


BlobHistory::BlobHistory( int _maxlen ):
	std::deque<BasicBlob>(),
	recent(0),
	newblob(1),
	total(0),
	maxlen(_maxlen)
{ }

void BlobHistory::add( BasicBlob& val ) {

	if (total == 0) first = val;
	total++;

	if (size() >= (unsigned int)maxlen) {
		pop_back();
	}

	pos = val.pos;
	push_front(val);

	recent = 1;
}

