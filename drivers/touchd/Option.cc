/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <Option.h>

Option::Option( double _init, double _min, double _max ):
	min(_min), max(_max), value(_init)
{ }
		
double Option::get() const { return value; }

void Option::set( double _value ) {
	value = _value;
	if (value > max) value = max;
	if (value < min) value = min;
}

void Option::inc() { set(value+1); }
void Option::dec() { set(value-1); }

