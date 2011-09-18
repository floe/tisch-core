/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _RELATIVEAXISSCALE_H_
#define _RELATIVEAXISSCALE_H_

#include "Scale.h"

#include <Vector.h>

class TISCH_SHARED RelativeAxisScale: public Scale {

	public:

		 RelativeAxisScale( unsigned int tf = INPUT_TYPE_ANY );
		~RelativeAxisScale();

		RelativeAxisScale* clone() const;

		void load( InputState& state );

		const char* name() const { return "RelativeAxisScale"; }

};

#endif // _RELATIVEAXISSCALE_H_
