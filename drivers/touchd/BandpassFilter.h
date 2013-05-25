/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BANDPASSFILTER_H_
#define _BANDPASSFILTER_H_

#include "Filter.h"

class BandpassFilter: public Filter {
	public:
		BandpassFilter( TiXmlElement* _config = 0, Filter* _input = 0 );

		int process();
		const char* name() const { return "BandpassFilter"; }
};

#endif // _BANDPASSFILTER_H_
