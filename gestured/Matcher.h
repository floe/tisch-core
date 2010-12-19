/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2010 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <map>
#include <set>

#include <BasicBlob.h>
#include <Factory.h>
#include <Region.h>
#include <Thread.h>


typedef std::deque<StateRegion*> RegionList;

class Matcher: public Thread {

	public:
		
		Matcher( int _verbose = 0 );

		virtual void request_update( int id ) = 0;
		virtual void trigger_gesture( int id, Gesture* g ) = 0;

		void process_blob( BasicBlob& blob );
		void process_gestures();
		
		void* run();

		void update( unsigned int id, Region* r );
		void remove( unsigned int id );

		void raise( unsigned int id );
		void lower( unsigned int id );

		void clear();

	protected:

		RegionList::iterator find( unsigned int id );

		int verbose, do_run;

		std::map<int,StateRegion*> stickies;
		RegionList regions;

		std::set<int> cur_ids;
		std::set<int> old_ids;

		std::set<StateRegion*> needs_update;
};

