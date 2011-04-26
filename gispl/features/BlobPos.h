/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBPOS_H_
#define _BLOBPOS_H_

#include "Feature.h"


class TISCH_SHARED BlobPos: public Feature<Vector> {

	public:

		 BlobPos( int tf = (1<<INPUT_TYPE_COUNT)-1 );
		~BlobPos();

		void load( InputState& state );
		int next();

		const char* name() const { return "BlobPos"; }

		std::vector<Vector> ids;
		std::vector<Vector>::iterator cur;
};

#endif // _BLOBPOS_H_

