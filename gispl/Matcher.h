/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MATCHER_H_
#define _MATCHER_H_

#include <map>
#include <set>

#include <TUIOInStream.h>
#include <BasicBlob.h>
#include <Factory.h>
#include <Region.h>
#include <Thread.h>


typedef std::deque<StateRegion*> RegionList;

class TISCH_SHARED Matcher: public Thread {

	public:
		
		Matcher( int _verbose = 0 );

		virtual void request_update( unsigned long long id ) = 0;
		virtual void trigger_gesture( unsigned long long id, Gesture* g ) = 0;

		virtual void process_blob( BasicBlob& blob );
		virtual void process_gestures();

		void load_defaults( unsigned int set = 0 );
		void* run();

		void update( unsigned long long id, Region* r );
		void remove( unsigned long long id );

		void raise( unsigned long long id );
		void lower( unsigned long long id );

		void peakmode( bool _use_peak );
		void clear();

	protected:

		RegionList::iterator find( unsigned long long id );

		int verbose;
		bool do_run, use_peak;

		std::map<int,StateRegion*> stickies;
		RegionList regions;

		std::set<int> cur_ids;
		std::set<int> old_ids;

		std::set<StateRegion*> needs_update;
};


class TISCH_SHARED MatcherTUIOInput: public TUIOInStream {

	public:

		MatcherTUIOInput( Matcher* m );
	
		void process_frame();
		void process_blob( BasicBlob& b );

	protected:

		Matcher* matcher;
};

#endif // _MATCHER_H_

