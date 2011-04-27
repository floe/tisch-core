/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _FEATURE_H_
#define _FEATURE_H_

#include <stdexcept>
#include <iostream>
#include <typeinfo>
#include <string>
#include <vector>
#include <deque>
#include <map>

#include "InputState.h"


class TISCH_SHARED FeatureBase {

	public:

		FeatureBase( int _tf = 0 ): has_result( 0 ), typeflags( _tf ) { } 
		virtual ~FeatureBase() { }

		virtual const char* name() const = 0; 
		virtual void load( InputState& state ) = 0;
		virtual int next() = 0;

		virtual void serialize( std::ostream& s ) = 0;
		virtual void unserialize( std::istream& s ) = 0;

		int has_result;

	protected:

		int typeflags;
};


template< class Value > class Feature: public FeatureBase {

	public:

		Feature( int _tf = (1<<INPUT_TYPE_COUNT)-1 ):
			FeatureBase( _tf ),
			m_result(),
			m_bounds()
		{ }

		virtual ~Feature() { }


		Value result() { return m_result; }

		std::vector<Value>& bounds( ) { // load a feature template 
			has_result = 0;
			return m_bounds;
		}

		// if this is a multi-match feature, overwrite to advance to the next match
		int next() {
			has_result = 0;
			return 0;
		}
		
		// load a feature instance
		virtual void load( InputState& state ) = 0;

		// return feature name
		virtual const char* name() const = 0;

		void serialize( std::ostream& s ) {
			s << "{\n\"type\":\"" << name() << "\",\n";
			//s << has_result << " ";
			s << "\"filters\":" << typeflags << ",\n";
			s << "\"constraints\":[";
			if (!has_result) {
				int size = m_bounds.size();
				for (int i = 0; i < size; i++ ) { 
					s << m_bounds[i];
					if (i < size-1) s << ",";
				}
			}
			s << "],\n";
			s << "\"result\":";
			if (has_result) s << m_result; else s << "[]";
			s << "\n}";
		}

		void unserialize( std::istream& s ) {
			char name[1024];
			s >> std::ws; s.getline(name,sizeof(name),':'); if (std::string("\"filters\"")     != name) throw std::runtime_error(name);
			s >> typeflags; s.ignore(1);
			s >> std::ws; s.getline(name,sizeof(name),':'); if (std::string("\"constraints\"") != name) throw std::runtime_error(name);
			s.ignore(1);
			while (s.peek() != ']') {
				Value tmp;
				s >> tmp;
				m_bounds.push_back(tmp);
				if (s.peek() == ',') s.ignore(1);
			}
			s.ignore(2);
			s >> std::ws; s.getline(name,sizeof(name),':'); if (std::string("\"result\"") != name) throw std::runtime_error(name);
			if (s.peek() != '[') {
				s >> m_result;
				has_result = 1;
				s >> std::ws;
				s.ignore(1);
			} else {
				has_result = 0;
				s.ignore(2);
				s >> std::ws;
				s.ignore(1);
			}
		}

	protected:

		Value m_result;

		std::vector<Value> m_bounds;
};

#endif // _FEATURE_H_

