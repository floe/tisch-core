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
#include <Socket.h>
#include <Thread.h>
#include <tisch.h>

#include <nanolibc.h>

#include <osc/OscReceivedElements.h>
#include <osc/OscPacketListener.h>
#include <ip/UdpSocket.h>

// 9 ms timeout + 1 ms delay ~ 100 Hz
struct timeval tv = { 0, 9000 };

int verbose = 0;
bool use_peak = false;

UDPSocket blobsrc( INADDR_ANY, TISCH_PORT_CALIB, &tv );
TCPSocket  gstsrc( INADDR_ANY, TISCH_PORT_EVENT, &tv );
TCPSocket* gstcon = 0;

typedef std::deque< StateRegion* > RegionList;
RegionList regions;

std::map<int,StateRegion*> stickies;

// default gestures as parseable string
const char* defaults = "region 1 0 0 6 \
	move 5 1 Motion 0 31 0 0 0 0 \
	scale 5 1 RelativeAxisScale 0 31 0 0 \
	rotate 5 1 RelativeAxisRotation 0 31 0 0 \
	tap 6 2 BlobID 0 27 0 0 BlobPos 0 27 0 0 0 0 \
	remove 6 1 BlobID 0 31 0 1 -1 \
	release 6 1 BlobCount 0 31 0 2 0 0 \
";

std::set<int> cur_ids;
std::set<int> old_ids;

std::set<StateRegion*> needs_update;


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
			use_peak = false;
			if (verbose) std::cout << "client signoff - flushing all regions." << std::endl;
			regions.clear();
			return -1;
		}

		//use blob peak instead of centroid
		if (cmd == "use_peak") {
			use_peak = true;
			return 1;
		}

		// see if we need to change an existing region..
		RegionList::iterator reg = regions.begin();
		for ( ; reg != regions.end(); reg++ )
			if ((*reg)->id == id)
				break;

		// raise the region to the top of the stack
		if (cmd == "raise") {
			if (reg != regions.end()) {
				if (verbose > 1) std::cout << "raise region " << (*reg)->id << std::endl;
				StateRegion* tmp = *reg;
				regions.erase( reg );
				regions.push_back( tmp );
			} else std::cout << "warning: trying to raise non-existent region " << id << std::endl;
			return 1;
		}

		// lower the region to the bottom of the stack
		if (cmd == "lower") {
			if (reg != regions.end()) {
				if (verbose > 1) std::cout << "lower region " << (*reg)->id << std::endl;
				StateRegion* tmp = *reg;
				regions.erase( reg );
				regions.push_front( tmp );
			} else std::cout << "warning: trying to lower non-existent region " << id << std::endl;
			return 1;
		}

		if (cmd != "region") {
			std::cout << "warning: unknown command '" << cmd << "'" << std::endl; 
			return 0;
		}

		// if this is a new region, allocate some storage
		if (reg == regions.end()) {
			regions.push_back( new StateRegion( id ) );
			reg = regions.end(); reg--;
		}

		// parse the region descriptor
		src >> **reg;

		// delete region & exit when error or empty
		if ((!src) || ((*reg)->size() == 0)) {

			// if this region was used as sticky target, erase that, too
			std::map<int,StateRegion*>::iterator sticky = stickies.begin();
			while (sticky != stickies.end()) {
				if (sticky->second == *reg) 
					stickies.erase( sticky++ );
				else
					sticky++;
			}

			delete *reg;
			regions.erase( reg );
			
			if (!src) return 0; else return 1;
		}

		if (verbose > 1) std::cout << "got region " << id << " " << **reg << std::endl;

		return 1;
	}

	void* run() {

		std::istringstream defstream( defaults );
		process( defstream );

		while (1) {

			if (!gstcon) { 
				gstcon = gstsrc.listen();
				if (verbose) std::cout << "client connected." << std::endl;
			}

			lock();

			int ret = process( *gstcon );

			switch (ret) {
				case  1: gstcon->clear(); break;
				case -1: delete gstcon; gstcon = 0; break;
				case  0: gstcon->flush(); std::cout << "Warning: stream error." << std::endl; break;
			}

			release();

			usleep(1000); // necessary to avoid starving the other thread
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


struct ReceiverThread : public osc::OscPacketListener 
{
	
virtual void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint )
{
	if( strcmp( m.AddressPattern(), "/tuio2/frm" ) == 0 ) 
	{
		osc::ReceivedMessageArgumentStream args = m.ArgumentStream();

		int update = 0;
		osc::int32 framenum;

		args >> framenum;

		// check for active client connection
		if (!gstcon) return;

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

			gthr.lock();

			// add all volatile regions to update list
			for (RegionList::reverse_iterator reg = regions.rbegin(); reg != regions.rend(); reg++)
				if ((*reg)->flags() & REGION_FLAGS_VOLATILE)
					needs_update.insert( *reg );

			// add all stickies to update list
			for (std::map<int,StateRegion*>::iterator sticky = stickies.begin(); sticky != stickies.end(); sticky++)
				needs_update.insert( sticky->second );

			// update all (ex-)stickies and all volatiles
			for (std::set<StateRegion*>::iterator reg = needs_update.begin(); reg != needs_update.end(); reg++) {
				if (verbose) std::cout << "requesting update of " << (*reg)->id << std::endl;
				*gstcon << "update " << (*reg)->id << std::endl;
			}

			needs_update.clear();

			gthr.release();

			// TODO: sleep? if so, how long?
			usleep( 5000 );
		}

		// cycle ids
		old_ids = cur_ids;
		cur_ids.clear();

		gthr.lock();

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
				*gstcon << "gesture " << (*reg)->id << " " << *gst << std::endl;
				if (verbose > 2) std::cout << "recognized a gesture: " << (*reg)->id << " " << *gst << std::endl;

				if (gst->flags() & GESTURE_FLAGS_STICKY) {
					// now add all blob ids from the current input state to stickies..
					// TODO: do this for all types of input state
					// TODO: only for matching blobs (possible in any way?)
					for (std::map<int,BlobHistory>::iterator ids = (*reg)->state[INPUT_TYPE_FINGER].begin(); ids != (*reg)->state[INPUT_TYPE_FINGER].end(); ids++) stickies[ids->first] = *reg;
				}
			}
		}

		// announce end of this block
		if (verbose > 2) std::cout << "finished processing gestures." << std::endl;

		gthr.release();

		return;
	}

	// find matching input type
	int input_type = -1;

	BasicBlob blob;
//	/tuio2/ptr s_id tu_id c_id x_pos y_pos width press [x_vel y_vel m_acc] 
	if( strcmp( m.AddressPattern(), "/tuio2/ptr" ) == 0 ) //finger
	{
		input_type = 0;
		osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
		osc::int32 objectid;
		osc::int32 unusedid;
		float posx, posy, width, press;
		args >> unusedid >> unusedid >> objectid >> posx >> posy >> width >> press;
		blob.id = objectid;
		blob.pos.x = posx;
		blob.pos.y = posy;
	}
//	/tuio2/tok s_id tu_id c_id x_pos y_pos angle [x_vel y_vel a_vel m_acc r_acc] 
	else if ( strcmp( m.AddressPattern(), "/tuio2/tok" ) == 0 ) //shadow
	{
		input_type = 2;
		osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
		osc::int32 objectid;
		osc::int32 unusedid;
		float posx, posy, angle;
		args >> unusedid >> unusedid >> objectid >> posx >> posy >> angle;
		blob.id = objectid;
		blob.pos.x = posx;
		blob.pos.y = posy;
	}
	//TODO additional information of blobs in content messages (prob. /tuio2/ctl)
	else if( strcmp( m.AddressPattern(), "/tuio2/alv" ) == 0 )
	{
		//call new function
		return;
	}

	cur_ids.insert( blob.id );

	gthr.lock();

	//if use_peak is set, use peak of blob instead of pos
	if (use_peak) blob.pos = blob.peak;

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

};

ReceiverThread receiver;

void quit( int i ) { exit(i); }

int main( int argc, char* argv[] ) {

	std::cout << "gestured - libTISCH 1.1 interpretation layer" << std::endl;
	std::cout << "(c) 2008 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	for ( int opt = 1; opt < argc; opt++ ) {
		if (std::string(argv[opt]) == "-v") verbose += 1;
	}

	//signal( SIGINT, quit );

	gthr.start();

	UdpListeningReceiveSocket s( IpEndpointName( IpEndpointName::ANY_ADDRESS, TISCH_PORT_CALIB ), &receiver );
	s.RunUntilSigInt();

}

