/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _PIPELINE2_H_
#define _PIPELINE2_H_

#include "Filter.h"


class Pipeline: public std::vector<Filter*> {

	public:

		 Pipeline( TiXmlElement* _config );
		~Pipeline();

		void createFilter( TiXmlElement* config, Filter* parent );

		int process();
		void reset();

};

#endif // _PIPELINE2_H_

