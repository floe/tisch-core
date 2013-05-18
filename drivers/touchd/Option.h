/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _OPTION_H_
#define _OPTION_H_

#include <map>
#include <string>

class Option {

	public:

		Option( int _init, int _min, int _max );
		
		int get();
		void set( int value );

		void inc();
		void dec();

	protected:

		int min, max, value;
};

typedef std::map<std::string,Option> OptionList;

#endif // _OPTION_H_

