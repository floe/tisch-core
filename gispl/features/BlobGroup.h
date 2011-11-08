/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBGROUP_H_
#define _BLOBGROUP_H_

#include "Feature.h"


class TISCH_SHARED BlobGroup: public Feature<Vector> {

	public:

		 BlobGroup( unsigned int tf = INPUT_TYPE_ANY );
		~BlobGroup();

		BlobGroup* clone() const;

		void load( InputState& state );
		int next();

		const char* name() const { return "BlobGroup"; }

	protected:

		std::vector<Vector> groups;
		std::vector<Vector>::iterator cur;
};

#endif // _BLOBGROUP_H_

