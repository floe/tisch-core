/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <signal.h>
#include <algorithm>
#include <sstream>
#include <map>
#include <set>

#include <Factory.h>
#include <Region.h>

#include <BasicBlob.h>
#include <UDPSocket.h>
#include <Thread.h>
#include <tisch.h>

#include <nanolibc.h>


struct timeval tv = { 0, 100000 };
int verbose = 0;

UDPSocket blobsrc( INADDR_ANY, TISCH_PORT_CALIB, &tv );
UDPSocket gstsrc ( INADDR_ANY, TISCH_PORT_EVENT );
UDPSocket gstsink( INADDR_ANY, 0 );

typedef std::deque< StateRegion* > RegionList;
RegionList regions;

// default gestures as parseable string
const char* defaults = "region 1 0 0 6 \
	move 5 1 Motion 0 31 0 0 0 0 \
	scale 5 1 RelativeAxisScale 0 31 0 0 \
	rotate 5 1 RelativeAxisRotation 0 31 0 0 \
	tap 6 2 BlobID 0 31 0 0 BlobPos 0 31 0 0 0 0 \
	remove 6 1 BlobID 0 31 0 1 -1 \
	release 6 1 BlobCount 0 31 0 2 0 0 \
";


struct GestureThread: public Thread {

	GestureThread(): Thread() { }

	int process( std::istream& src ) {

		std::string cmd;
		unsigned long long id;

		src >> cmd;
		src >> id;

		if (!src) return 0;

		lock();

		// wipe everything when the client quits
		if (cmd == "bye") {
			if (verbose) std::cout << "client signoff - flushing all regions." << std::endl;
			regions.clear();
			release();
			return 1;
		}

		// see if we need to change an existing region..
		RegionList::iterator reg = regions.begin();
		for ( ; reg != regions.end(); reg++ )
			if ((*reg)->id == id)
				break;

		// raise the region to the top of the stack
		if ((cmd == "raise") && (reg != regions.end())) {
			if (verbose) std::cout << "raising region " << (*reg)->id << std::endl;
			StateRegion* tmp = *reg;
			regions.erase( reg );
			regions.push_back( tmp );
			release();
			return 1;
		}

		if (cmd != "region") return 0;

		// if this is a new region, allocate some storage
		if (reg == regions.end()) {
			regions.push_back( new StateRegion( id ) );
			reg = regions.end(); reg--;
		}

		// parse the region descriptor
		src >> **reg;

		// delete region & exit when error or empty
		if ((!src) || ((*reg)->size() == 0)) {
			delete *reg;
			regions.erase( reg );
			release();
			return 0;
		}

		if (verbose >= 1) std::cout << "got region " << id << " " << **reg << std::endl;

		int port = 0;
		in_addr_t addr = gstsrc.source( &port );
		gstsink.target( addr, port );

		release();
		return 1;
	}

	void* run() {

		std::istringstream defstream( defaults );
		process( defstream );

		while (1) {
			if (!process( gstsrc ))
				gstsrc.flush();
		}
		return 0;
	}

};

GestureThread gthr;

// prinzipielles vorgehen:
//
// - für jeden eingabeblob:
//   - falls neue id: regionen updaten (done)
//   - prüfen, ob id als sticky markiert => in inputstate von originalregion einfügen (done)
//   - für restliche blobs: if region.contains(blob) == true => in inputstate einfügen (done)
//
// - danach:
//
// - für jede region:
//   - alle existierende features für inputstate berechnen ( done ) 
//   - für jede enthaltene geste:
//     - für jedes enthaltene feature: match() mit vorgabefeature, falls 1: assignment operator (done)
//     - wenn alle features assigned: geste senden (done)
//     - wenn geste sticky: in stickies einfügen

void quit( int i ) { exit(i); }

int main( int argc, char* argv[] ) {

	std::cout << "gestured - TISCH interpretation layer 1.0 beta1" << std::endl;
	std::cout << "(c) 2008 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	for ( int opt = 1; opt < argc; opt++ ) {
		if (std::string(argv[opt]) == "-v") verbose += 1;
	}

	signal( SIGINT, quit );

	gthr.start();

	std::set<int> cur_ids;
	std::set<int> old_ids;

	std::map<int,StateRegion*> stickies;
	std::set<StateRegion*> needs_update;


	while (1) {

		std::string type;
		blobsrc >> type;

		// flush on error
		if (!blobsrc) { blobsrc.flush(); continue; }

		if (type == "frame") {

			int update = 0;
			int framenum;

			blobsrc >> framenum;
			if (!blobsrc) { blobsrc.flush(); continue; }

			// remove vanished IDs from stickies, add to update list
			for (std::set<int>::iterator id = old_ids.begin(); id != old_ids.end(); id++)
				if (cur_ids.find(*id) == cur_ids.end()) {

					std::map<int,StateRegion*>::iterator sticky = stickies.find( *id );
					if (sticky == stickies.end()) continue;

					needs_update.insert( sticky->second ); //stickies[-(*id)] = sticky->second;
					stickies.erase( sticky );
				}

			// check for new ids since last frame
			for (std::set<int>::iterator id = cur_ids.begin(); id != cur_ids.end(); id++)
				if (old_ids.find(*id) == old_ids.end())
					update = 1;

			// new ids found: request region update and clear stickies
			if (update) {

				// add all volatile regions to update list
				gthr.lock();
				for (RegionList::reverse_iterator reg = regions.rbegin(); reg != regions.rend(); reg++)
					if ((*reg)->flags() & REGION_FLAGS_VOLATILE)
						needs_update.insert( *reg );
				gthr.release();

				// add all stickies to update list
				for (std::map<int,StateRegion*>::iterator sticky = stickies.begin(); sticky != stickies.end(); sticky++)
					needs_update.insert( sticky->second );

				// update all (ex-)stickies and all volatiles
				for (std::set<StateRegion*>::iterator reg = needs_update.begin(); reg != needs_update.end(); reg++) {
					if (verbose) std::cout << "requesting update of " << (*reg)->id << std::endl;
					gstsink << "update " << (*reg)->id << std::endl;
				}

				// TODO: no danger of stickies overflowing?
				//stickies.clear();
				needs_update.clear();

				// TODO: sleep? if so, how long?
				//usleep( 5000 );
			}

			// cycle ids
			old_ids = cur_ids;
			cur_ids.clear();

			gthr.lock();

			// announce start of this block
			if (verbose >= 2) std::cout << "start processing gestures:" << std::endl;

			// loop over all registered regions
			for (RegionList::reverse_iterator reg = regions.rbegin(); reg != regions.rend(); reg++) {

				Gesture* gst = 0;

				// no input data available -> go to next region
				//if (!(*reg)->state.changed()) continue;

				// wipe old blobs & reset flags
				(*reg)->state.purge(); // TODO: split into check & purge

				// update all features for this region from inputstate
				(*reg)->update();

				// iterate over all matching gestures
				while ((gst = (*reg)->nextMatch())) {

					// check the oneshot flag
					if (gst->flags() & GESTURE_FLAGS_ONESHOT) {
						// even if a match occured, only send if the input ids have changed
						if (!(*reg)->state.changed()) continue;
					}

					// transmit the current gesture along with the matched feature instances
					gstsink << "gesture " << (*reg)->id << " " << *gst << std::endl;
					if (verbose >= 2) std::cout << "recognized a gesture: " << (*reg)->id << " " << *gst << std::endl;

					if (gst->flags() & GESTURE_FLAGS_STICKY) {
						// now add all blob ids from the current input state to stickies..
						// TODO: do this for all types of input state
						// TODO: only for matching blobs (possible in any way?)
						for (std::map<int,BlobHistory>::iterator ids = (*reg)->state[INPUT_TYPE_FINGER].begin(); ids != (*reg)->state[INPUT_TYPE_FINGER].end(); ids++) stickies[ids->first] = *reg;
					}
				}
			}

			// announce end of this block
			if (verbose >= 2) std::cout << "finished processing gestures." << std::endl;

			gthr.release();

			continue; 
		}

		// find matching input type
		int input_type = -1;
		for (int i = 0; i < INPUT_TYPE_COUNT; i++)
			if (type == inputname[i]) input_type = i;

		if (input_type == -1) {

			blobsrc.flush();
			continue;

		} else {

			BasicBlob blob;
			blobsrc >> blob;

			if (!blobsrc) { blobsrc.flush(); continue; }

			cur_ids.insert( blob.id );

			gthr.lock();

			// insert blob into correct region 
			std::map<int,StateRegion*>::iterator target = stickies.find( blob.id );
			if (target != stickies.end()) {
				// blob is sticky, so add to the previous region
				target->second->state[input_type][blob.id].add( blob );
				//std::cout << "adding blob " << blob.id << " to region " << target->first << std::endl;
			}
			else
				for (RegionList::reverse_iterator reg = regions.rbegin(); reg != regions.rend(); reg++)
					// check all regions and insert blob into first match
					if ((*reg)->contains( blob.pos )) {
						// also check type flags (is the blob transparent to this object type?)
						if ((*reg)->flags() & (1<<input_type)) {
							(*reg)->state[input_type][blob.id].add( blob );
							//std::cout << "adding blob type " << input_type << " with id " << blob.id << " to region " << (*reg)->id << " with flags " << (*reg)->flags() << std::endl;
							break;
						}
					}

			gthr.release();

		}
	}
}

