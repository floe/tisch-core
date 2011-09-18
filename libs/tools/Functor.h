/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _FUNCTOR_H_
#define _FUNCTOR_H_

#include "SmartPtr.h"


// FunctorX implementations with X parameters - currently X e [0;4]

template < typename Res > class Functor0 {

	public:

		Functor0( Res (*func)() ): m_func(func) { }
		virtual ~Functor0() { }

		virtual Res operator()() { return m_func(); }

	protected:

		Functor0() { }

		Res (*m_func)();

};

template < typename Res, typename P1 > class Functor1: public Functor0<Res> {

	public:

		Functor1( Res (*func)(P1), P1 p1 ): m_func(func), m_p1(p1) { }
		virtual Res operator()() { return m_func( m_p1 ); }

	protected:

		Functor1( P1 p1 ): m_p1(p1) { }

		Res (*m_func)( P1 );
		P1    m_p1;

};

template < typename Res, typename P1, typename P2 > class Functor2: public Functor1<Res,P1> {

	public:

		Functor2( Res (*func)(P1,P2), P1 p1, P2 p2 ): Functor1<Res,P1>(p1), m_func(func), m_p2(p2) { }
		virtual Res operator()() { return m_func( this->m_p1, m_p2 ); }

	protected:

		Functor2( P1 p1, P2 p2 ): Functor1<Res,P1>(p1), m_p2(p2) { }

		Res (*m_func)( P1, P2 );
		P2    m_p2;

};

template < typename Res, typename P1, typename P2, typename P3 > class Functor3: public Functor2<Res,P1,P2> {

	public:

		Functor3( Res (*func)(P1,P2,P3), P1 p1, P2 p2, P3 p3 ): Functor2<Res,P1,P2>(p1,p2), m_func(func), m_p3(p3) { }
		virtual Res operator()() { return m_func( this->m_p1, this->m_p2, m_p3 ); }

	protected:

		Functor3( P1 p1, P2 p2, P3 p3 ): Functor2<Res,P1,P2>(p1,p2), m_p3(p3) { }

		Res (*m_func)( P1, P2, P3 );
		P3    m_p3;

};

template < typename Res, typename P1, typename P2, typename P3, typename P4 > class Functor4: public Functor3<Res,P1,P2,P3> {

	public:

		Functor4( Res (*func)(P1,P2,P3,P4), P1 p1, P2 p2, P3 p3, P4 p4 ): Functor3<Res,P1,P2,P3>(p1,p2,p3), m_func(func), m_p4(p4) { }
		virtual Res operator()() { return m_func( this->m_p1, this->m_p2, this->m_p3, m_p4 ); }

	protected:

		Functor4( P1 p1, P2 p2, P3 p3, P4 p4 ): Functor3<Res,P1,P2,P3>(p1,p2,p3), m_p4(p4) { }

		Res (*m_func)( P1, P2, P3, P4 );
		P4    m_p4;

};


// the Functor frontend
template < typename Res > class Functor {

	public:

		Functor( Functor0<Res>* func ): m_func(func) { }
		Res operator()() { return (*m_func)(); }

	protected:

		SmartPtr< Functor0<Res> > m_func;

};


// polymorphic bind() functions for variable parameter count
template <
	typename Res
>
Functor<Res> bind( Res (*func)() ) { 
	return Functor<Res>( new Functor0<Res>( func ) );
}

template <
	typename Res,
	typename P1,
	typename A1
>
Functor<Res> bind( Res (*func)(P1), A1 p1 ) {
	return Functor<Res>( new Functor1<Res,P1>( func, p1 ) ); 
}

template <
	typename Res,
	typename P1, typename P2,
	typename A1, typename A2
> 
Functor<Res> bind( Res (*func)(P1,P2), A1 p1, A2 p2 ) {
	return Functor<Res>( new Functor2<Res,P1,P2>( func, p1, p2 ) );
}

template <
	typename Res,
	typename P1, typename P2, typename P3,
	typename A1, typename A2, typename A3
> 
Functor<Res> bind( Res (*func)(P1,P2,P3), A1 p1, A2 p2, A3 p3 ) {
	return Functor<Res>( new Functor3<Res,P1,P2,P3>( func, p1, p2, p3 ) );
}

template <
	typename Res, 
	typename P1, typename P2, typename P3, typename P4, 
	typename A1, typename A2, typename A3, typename A4
> 
Functor<Res> bind( Res (*func)(P1,P2,P3,P4), A1 p1, A2 p2, A3 p3, A4 p4 ) {
	return Functor<Res>( new Functor4<Res,P1,P2,P3,P4>( func, p1, p2, p3, p4 ) );
}


#endif // _FUNCTOR_H_

