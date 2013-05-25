/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _THRESHFILTER_H_
#define _THRESHFILTER_H_

#include "Filter.h"

class ThreshFilter: public Filter {
	public:
		ThreshFilter( TiXmlElement* _config = 0, Filter* _input = 0 );

		virtual int process();
		const char* name() const { return "ThreshFilter"; }
};

#endif // _THRESHFILTER_H_
