/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _PIPELINE2_H_
#define _PIPELINE2_H_

#include "AreaFilter.h"
#include "BGSubFilter.h"


class Pipeline: public std::vector<Filter*> {

	public:

		 Pipeline( TiXmlElement* _config );
		~Pipeline();

		void createFilter( TiXmlElement* config, Filter* parent );

		int process();
		void reset(int initialReset);

		// storing configuration
		void storeXMLConfig(std::string storingTarget);
		TiXmlElement* getXMLSubTree(int startIndex, Filter* parentOfRoot);

};

#endif // _PIPELINE2_H_
