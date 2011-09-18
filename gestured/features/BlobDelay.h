/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBDELAY_H_
#define _BLOBDELAY_H_

#include "Feature.h"


class TISCH_SHARED BlobDelay: public Feature<int> {

	public:

		 BlobDelay( unsigned int tf = INPUT_TYPE_ANY );
		~BlobDelay();

		BlobDelay* clone() const;

		void load( InputState& state );

		const char* name() const { return "BlobDelay"; }
};

#endif // _BLOBDELAY_H_

