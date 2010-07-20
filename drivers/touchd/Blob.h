/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BLOB_H_
#define _BLOB_H_

#include <IntensityImage.h>
#include <BasicBlob.h>

#include "Settings.h"

class Blob: public BasicBlob {

	friend class BlobList;

	public:

		Blob( IntensityImage* image, Point seed, unsigned char value, BlobSettings* set );

		unsigned char scan( IntensityImage* image, double factor );

		void setPeak( IntensityImage* image, double factor = 1.4, double maxdist = 100 );

};

#endif // _BLOB_H_

