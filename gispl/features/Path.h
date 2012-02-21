/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _PATH_H_
#define _PATH_H_

#include "Feature.h"

#include <Vector.h>

class TISCH_SHARED Path: public Feature<Vector> {

	public:

		 Path( unsigned int tf = INPUT_TYPE_ANY );
		~Path();

		Path* clone() const;

		void load( InputState& state );

		const char* name() const { return "Path"; }

	private:

		std::vector<Vector> path;

};

#endif // _PATH_H_

