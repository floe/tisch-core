/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBCOUNT_H_
#define _BLOBCOUNT_H_

#include "Feature.h"


class TISCH_SHARED BlobCount: public Feature<int> {

	public:

		 BlobCount( unsigned int tf = INPUT_TYPE_ANY );
		~BlobCount();

		BlobCount* clone() const;

		void load( InputState& state );

		const char* name() const { return "BlobCount"; }
};

#endif // _BLOBCOUNT_H_

