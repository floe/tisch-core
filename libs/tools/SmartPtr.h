/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _SMARTPTR_H_
#define _SMARTPTR_H_

// This class is a slightly modified version of counted_ptr.h from
// http://ootips.org/yonat/4dev/ by Yonat Sharon. Thanks, Yonat!

template <class X> class SmartPtr {

	public:

		SmartPtr( X* p = 0 ): itsCounter(0) { // allocate a new counter, could be explicit
			if (p) itsCounter = new counter(p);
		}

		~SmartPtr() {
			release();
		}

		SmartPtr( const SmartPtr& r ) throw() {
			acquire( r.itsCounter );
		}

		SmartPtr& operator=( const SmartPtr& r ) {
			if (this != &r) {
				release();
				acquire( r.itsCounter );
			}
			return *this;
		}

		   operator X*() const throw() { return  itsCounter->ptr; }
		X& operator  *() const throw() { return *itsCounter->ptr; }
		X* operator ->() const throw() { return  itsCounter->ptr; }
		X* get()         const throw() { return  itsCounter ? itsCounter->ptr : 0; }
		bool unique()    const throw() { return (itsCounter ? itsCounter->count == 1 : true); }

	private:

		struct counter {

			counter( X* p = 0, unsigned c = 1 ): ptr(p), count(c) { }

			X*        ptr;
			unsigned  count;

		}* itsCounter;

		void acquire(counter* c) throw() { // increment the count
			itsCounter = c;
			if (c) ++c->count;
		}

		void release() { // decrement the count, delete if it is 0
			if (itsCounter) {
				if (--itsCounter->count == 0) {
					delete itsCounter->ptr;
					delete itsCounter;
				}
				itsCounter = 0;
			}
		}
};

#endif // _SMARTPTR_H_

