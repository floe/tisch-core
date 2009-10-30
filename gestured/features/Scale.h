/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _SCALE_H_
#define _SCALE_H_

#include "Feature.h"

#include <Vector.h>

class TISCH_SHARED Scale: public Feature<double> {

	public:

		 Scale( int tf = (1<<INPUT_TYPE_COUNT)-1 );
		~Scale();

		void load( InputState& state );

		const char* name() const { return "Scale"; }

};

#endif // _SCALE_H_

