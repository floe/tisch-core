/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _SPLITFILTER_H_
#define _SPLITFILTER_H_

#include "Filter.h"

class SplitFilter: public Filter {
	public:
		SplitFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual int process();
		virtual void reset();
		virtual IntensityImage* getImage();
		virtual TiXmlElement* getXMLRepresentation();
	protected:
		IntensityImage* image2;
		int incount, outcount;
};

#endif // _SPLITFILTER_H_
