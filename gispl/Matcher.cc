/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Matcher.h"
#include <sstream>


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


// default gesture sets as parseable strings
TISCH_SHARED const char* default_gesture_set[] = {
	"0 0 6 \
		{ \"name\":\"move\",    \"flags\":5, \"features\":[{ \"type\":\"Motion\", \"filters\":31, \"constraints\":[], \"result\":[]}] } \
		{ \"name\":\"scale\",   \"flags\":5, \"features\":[{ \"type\":\"MultiBlobScale\", \"filters\":31, \"constraints\":[], \"result\":[]}] } \
		{ \"name\":\"rotate\",  \"flags\":5, \"features\":[{ \"type\":\"MultiBlobRotation\", \"filters\":31, \"constraints\":[], \"result\":[]}] } \
		{ \"name\":\"tap\",     \"flags\":6, \"features\":[{ \"type\":\"BlobID\", \"filters\":31, \"constraints\":[], \"result\":[]}, { \"type\":\"BlobPos\", \"filters\":31, \"constraints\":[], \"result\":[]}] } \
		{ \"name\":\"remove\",  \"flags\":6, \"features\":[{ \"type\":\"BlobID\", \"filters\":31, \"constraints\":[ -1 ], \"result\":[]}] } \
		{ \"name\":\"release\", \"flags\":6, \"features\":[{ \"type\":\"BlobCount\", \"filters\":31, \"constraints\":[ 0, 0 ], \"result\":[]}] }",
	"0 0 6 \
		{ \"name\":\"move\",    \"flags\":5, \"features\":[{ \"type\":\"Motion\", \"filters\":31, \"constraints\":[], \"result\":[]}] } \
		{ \"name\":\"scale\",   \"flags\":5, \"features\":[{ \"type\":\"RelativeAxisScale\", \"filters\":31, \"constraints\":[], \"result\":[]}] } \
		{ \"name\":\"rotate\",  \"flags\":5, \"features\":[{ \"type\":\"RelativeAxisRotation\", \"filters\":31, \"constraints\":[], \"result\":[]}] } \
		{ \"name\":\"tap\",     \"flags\":6, \"features\":[{ \"type\":\"BlobID\", \"filters\":31, \"constraints\":[], \"result\":[]}, { \"type\":\"BlobPos\", \"filters\":31, \"constraints\":[], \"result\":[]}] } \
		{ \"name\":\"remove\",  \"flags\":6, \"features\":[{ \"type\":\"BlobID\", \"filters\":31, \"constraints\":[ -1 ], \"result\":[]}] } \
		{ \"name\":\"release\", \"flags\":6, \"features\":[{ \"type\":\"BlobCount\", \"filters\":31, \"constraints\":[ 0, 0 ], \"result\":[]}] }"
};


Matcher::Matcher( int _v ): Thread(), verbose(_v), do_run(1), use_peak(0) { }


void Matcher::load_defaults( unsigned int set ) {
	if (set > (sizeof(default_gesture_set)/sizeof(const char*))-1) set = 0;
	std::istringstream defstream( default_gesture_set[set] );
	Region tmp; defstream >> tmp;
	update( 0, &tmp );
}

void* Matcher::run() {
	while (do_run) process_gestures();
	return NULL;
}


RegionList::iterator Matcher::find( unsigned long long id ) {
	RegionList::iterator reg = regions.begin();
	for ( ; reg != regions.end(); reg++ )
		if ((*reg)->id == id)
			break;
	return reg;
}


void Matcher::update( unsigned long long id, Region* r ) {
	lock();
	RegionList::iterator reg = find( id );
	if (reg != regions.end()) {
		**reg = *r;
	} else {
		StateRegion* tmp = new StateRegion( id );
		*tmp = *r;
		reg = regions.insert( regions.end(), tmp );
	}
	for (std::vector<Gesture>::iterator g = (*reg)->gestures.begin(); g != (*reg)->gestures.end(); g++) g->check();
	if (verbose > 1) std::cout << "region: " << id << " = " << **reg << std::endl;
	release();
}

void Matcher::remove( unsigned long long id ) {
	lock();
	RegionList::iterator reg = find( id );
	if (reg != regions.end()) {
		std::map<int,StateRegion*>::iterator sticky = stickies.begin();
		while (sticky != stickies.end()) {
			if (sticky->second == *reg)
				stickies.erase( sticky++ );
			else
				sticky++;
		}
		delete *reg;
		regions.erase( reg );
	}
	release();
}

void Matcher::raise( unsigned long long id ) {
	lock();
	RegionList::iterator reg = find( id );
	if (reg != regions.end()) {
		if (verbose > 1) std::cout << "raise region " << id << std::endl;
		StateRegion* tmp = *reg;
		regions.erase( reg );
		regions.push_back( tmp );
	} else std::cout << "warning: trying to raise non-existent region " << id << std::endl;
	release();
}

void Matcher::lower( unsigned long long id ) {
	lock();
	RegionList::iterator reg = find( id );
	if (reg != regions.end()) {
		if (verbose > 1) std::cout << "lower region " << id << std::endl;
		StateRegion* tmp = *reg;
		regions.erase( reg );
		regions.push_front( tmp );
	} else std::cout << "warning: trying to lower non-existent region " << id << std::endl;
	release();
}

void Matcher::peakmode( bool _use_peak ) {
	use_peak = _use_peak;
}

void Matcher::clear() {
	lock();
	regions.clear();
	stickies.clear();
	release();
	use_peak = 0;
}


void Matcher::process_blob( BasicBlob& blob ) {

	lock();

	cur_ids.insert( blob.id );

	// if use_peak is set, use peak of blob instead of pos
	if (use_peak) blob.pos = blob.peak;

	// insert blob into correct region 
	std::map<int,StateRegion*>::iterator target = stickies.find( blob.id );
	if (target != stickies.end()) {
		// blob is sticky, so add to the previous region
		target->second->state[blob.type][blob.id].add( blob );
		//std::cout << "adding blob " << blob.id << " to region " << target->first << std::endl;
	} else {
		for (RegionList::reverse_iterator reg = regions.rbegin(); reg != regions.rend(); reg++) {
			// check all regions and insert blob into first match
			if ((*reg)->contains( blob.pos )) {
				// also check type flags (is the blob transparent to this object type?)
				if ((*reg)->flags() & (1<<blob.type)) {
					(*reg)->state[blob.type][blob.id].add( blob );
					//std::cout << "adding blob type " << blob.type << " with id " << blob.id << " to region ";
					//std::cout << (*reg)->id << " with flags " << (*reg)->flags() << std::endl;
					break;
				}
			}
		}
	}

	release();
}


void Matcher::prepare_updates() {

	lock();

	int update = 0;
	has_frame = true;

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
		for (RegionList::reverse_iterator reg = regions.rbegin(); reg != regions.rend(); reg++)
			if ((*reg)->flags() & REGION_FLAGS_VOLATILE)
				needs_update.insert( *reg );

		// add all stickies to update list
		for (std::map<int,StateRegion*>::iterator sticky = stickies.begin(); sticky != stickies.end(); sticky++)
			needs_update.insert( sticky->second );
	}

		// release semaphore so that incoming updates can be processed
		release();
}

void Matcher::process_updates() {

	lock();
	std::set<StateRegion*> tmp_update = needs_update;
	needs_update.clear();
	release();

		// update all (ex-)stickies and all volatiles
		for (std::set<StateRegion*>::iterator reg = tmp_update.begin(); reg != tmp_update.end(); reg++) {
			if (verbose) std::cout << "requesting update of " << (*reg)->id << std::endl;
			request_update( (*reg)->id );
		}
}

void Matcher::prepare_gestures() {

	// cycle ids
	old_ids = cur_ids;
	cur_ids.clear();

	lock();

	// announce start of this block
	if (verbose > 2) std::cout << "start processing gestures:" << std::endl;

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
			if (verbose > 2) std::cout << "recognized a gesture: " << (*reg)->id << " " << *gst << std::endl;
			action_queue.push_back( Action( (*reg)->id, gst ) );

			if (gst->flags() & GESTURE_FLAGS_STICKY) {
				// now add all blob ids from the current input state to stickies..
				// TODO: do this for all types of input state
				// TODO: only for matching blobs (possible in any way?)
				for (std::map<int,BlobHistory>::iterator ids = (*reg)->state[INPUT_TYPE_FINGER].begin(); ids != (*reg)->state[INPUT_TYPE_FINGER].end(); ids++)
					stickies[ids->first] = *reg;
			}
		}
	}

	// announce end of this block
	if (verbose > 2) std::cout << "finished processing gestures." << std::endl;

	release();
}

void Matcher::process_gestures() {
	while (!action_queue.empty()) {
		Action& a = action_queue.front();
		trigger_gesture( a.first, a.second );
		action_queue.pop_front();
	}
}

bool Matcher::has_input_data() {
	bool res = has_frame;
	has_frame = 0;
	return res;
}



MatcherTUIOInput::MatcherTUIOInput( Matcher* m ): TUIOInStream(), matcher(m) { }

void MatcherTUIOInput::process_frame() {
	matcher->prepare_updates();
	matcher->prepare_gestures();
}

void MatcherTUIOInput::process_blob( BasicBlob& b ) {
	matcher->process_blob( b );
}

