/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MULTIBLOBSCALE_H_
#define _MULTIBLOBSCALE_H_

#include "Scale.h"

#include <Vector.h>

class TISCH_SHARED MultiBlobScale: public Scale {

	public:

		 MultiBlobScale( unsigned int tf = INPUT_TYPE_ANY );
		~MultiBlobScale();

		MultiBlobScale* clone() const;

		void load( InputState& state );

		const char* name() const { return "MultiBlobScale"; }

};

#endif // _MULTIBLOBSCALE_H_

