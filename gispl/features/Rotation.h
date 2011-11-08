/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _ROTATION_H_
#define _ROTATION_H_

#include "Feature.h"

#include <Vector.h>

class TISCH_SHARED Rotation: public Feature<double> {

	public:

		 Rotation( unsigned int tf = INPUT_TYPE_ANY );
		~Rotation();

		Rotation* clone() const;

		void load( InputState& state );

		const char* name() const { return "Rotation"; }

};

#endif // _ROTATION_H_

