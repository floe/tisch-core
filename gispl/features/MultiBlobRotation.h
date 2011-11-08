/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MULTIBLOBROTATION_H_
#define _MULTIBLOBROTATION_H_

#include "Rotation.h"

class TISCH_SHARED MultiBlobRotation: public Rotation {

	public:

		 MultiBlobRotation( unsigned int tf = INPUT_TYPE_ANY );
		~MultiBlobRotation();

		MultiBlobRotation* clone() const;

		void load( InputState& state );

		const char* name() const { return "MultiBlobRotation"; }

};

#endif // _MULTIBLOBROTATION_H_

