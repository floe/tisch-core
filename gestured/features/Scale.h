/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _SCALE_H_
#define _SCALE_H_

#include "Feature.h"

#include <Vector.h>

class TISCH_SHARED Scale: public Feature<double> {

	public:

		 Scale( unsigned int tf = INPUT_TYPE_ANY );
		~Scale();

		Scale* clone() const;

		void load( InputState& state );

		const char* name() const { return "Scale"; }

};

#endif // _SCALE_H_

