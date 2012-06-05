/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOBMARKER_H_
#define _BLOBMARKER_H_

#include "Feature.h"


class TISCH_SHARED BlobMarker: public Feature<int> {

	public:

		 BlobMarker( unsigned int tf = INPUT_TYPE_ANY );
		~BlobMarker();

		BlobMarker* clone() const;

		void load( InputState& state );

		const char* name() const { return "BlobMarker"; }
	
		int markerID;
};

#endif // _BLOBMARKER_H_

