/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBPARENT_H_
#define _BLOBPARENT_H_

#include "Feature.h"


class TISCH_SHARED BlobParent: public Feature<int> {

	public:

		 BlobParent( int tf = (1<<INPUT_TYPE_COUNT)-1 );
		~BlobParent();

		BlobParent* clone() const;

		void load( InputState& state );
		int next();

		const char* name() const { return "BlobParent"; }

	protected:

		std::vector<int> ids;
		std::vector<int>::iterator cur;
};

#endif // _BLOBPARENT_H_

