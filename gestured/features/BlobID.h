/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBID_H_
#define _BLOBID_H_

#include "Feature.h"


class TISCH_SHARED BlobID: public Feature<int> {

	public:

		 BlobID( unsigned int tf = INPUT_TYPE_ANY );
		~BlobID();

		BlobID* clone() const;

		void load( InputState& state );
		int next();

		const char* name() const { return "BlobID"; }

	protected:

		std::vector<int> ids;
		std::vector<int>::iterator cur;
		int mode;
};

#endif // _BLOBID_H_

