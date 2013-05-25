/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _SPECKLEFILTER_H_
#define _SPECKLEFILTER_H_

#include "Filter.h"

class SpeckleFilter: public Filter {
	public:
		SpeckleFilter( TiXmlElement* _config = 0, Filter* _input = 0 );

		int process();
		const char* name() const { return "SpeckleFilter"; }
};

#endif // _SPECKLEFILTER_H_
