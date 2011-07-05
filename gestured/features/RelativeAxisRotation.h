/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _RELATIVEAXISROTATION_H_
#define _RELATIVEAXISROTATION_H_

#include "Rotation.h"

class TISCH_SHARED RelativeAxisRotation: public Rotation {

	public:

		 RelativeAxisRotation( int tf = (1<<INPUT_TYPE_COUNT)-1 );
		~RelativeAxisRotation();

		RelativeAxisRotation* clone() const;

		void load( InputState& state );

		const char* name() const { return "RelativeAxisRotation"; }

};

#endif // _RELATIVEAXISROTATION_H_

