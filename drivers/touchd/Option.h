/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _OPTION_H_
#define _OPTION_H_

#include <map>
#include <string>

#ifndef __UINT32_MAX__
#include <cstdint>
#define __UINT32_MAX__ ((uint32_t)-1)
#endif

class Option {

	public:

		Option() { }

		Option( double _init, double _min = 0, double _max = __UINT32_MAX__ );
		
		double get() const;
		void set( double value );

		void inc();
		void dec();

	protected:

		double min, max, value;
};

typedef std::map<std::string,Option*> OptionList;

#endif // _OPTION_H_

