/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBDIM_H_
#define _BLOBDIM_H_

#include "Feature.h"


struct Dimensions {
	double x1, y1, x2, y2, size;
};

TISCH_SHARED std::istream& operator>>( std::istream& s, Dimensions& d );
TISCH_SHARED std::ostream& operator<<( std::ostream& s, Dimensions& d );


class TISCH_SHARED BlobDim: public Feature<Dimensions> {

	public:

		 BlobDim( int tf = (1<<INPUT_TYPE_COUNT)-1 );
		~BlobDim();

		BlobDim* clone() const;

		void load( InputState& state );
		int next();

		const char* name() const { return "BlobDim"; }

	protected:

		int check();

		std::vector<Dimensions> ids;
		std::vector<Dimensions>::iterator cur;
};

#endif // _BLOBDIM_H_

