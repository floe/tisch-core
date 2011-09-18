/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MOTION_H_
#define _MOTION_H_

#include "Feature.h"

#include <Vector.h>

class TISCH_SHARED Motion: public Feature<Vector> {

	public:

		 Motion( unsigned int tf = INPUT_TYPE_ANY );
		~Motion();

		Motion* clone() const;

		void load( InputState& state );

		const char* name() const { return "Motion"; }

};

#endif // _MOTION_H_

