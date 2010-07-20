/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _RELATIVEAXISSCALE_H_
#define _RELATIVEAXISSCALE_H_

#include "Scale.h"

#include <Vector.h>

class TISCH_SHARED RelativeAxisScale: public Scale {

	public:

		 RelativeAxisScale( int tf = (1<<INPUT_TYPE_COUNT)-1 );
		~RelativeAxisScale();

		void load( InputState& state );

		const char* name() const { return "RelativeAxisScale"; }

};

#endif // _RELATIVEAXISSCALE_H_
