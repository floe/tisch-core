/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _FLIPFILTER_H_
#define _FLIPFILTER_H_

#include "Filter.h"

class FlipFilter: public Filter {
	public:
		FlipFilter( TiXmlElement* _config = 0, Filter* _input = 0 );

		int process();
		const char* name() const { return "FlipFilter"; }

};

#endif // _FLIPFILTER_H_
