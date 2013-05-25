/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _LOWPASSFILTER_H_
#define _LOWPASSFILTER_H_

#include "Filter.h"

class LowpassFilter: public Filter {
	public:
		LowpassFilter( TiXmlElement* _config = 0, Filter* _input = 0 );

		int process();
		const char* name() const { return "LowpassFilter"; }
};

#endif // _LOWPASSFILTER_H_
