/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _THRESHFILTER_H_
#define _THRESHFILTER_H_

#include "Filter.h"

class ThreshFilter: public Filter {
	public:
		ThreshFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual int process();
		// Configurator
		virtual const char* getOptionName(int option);
		virtual double getOptionValue(int option);
		virtual void modifyOptionValue(double delta, bool overwrite);
		virtual TiXmlElement* getXMLRepresentation();
	protected:
		int THRESH_MAX; // 255 if intensity image is used, else 2047
		// Options
		int threshold_min;
		int threshold_max;
};

#endif // _THRESHFILTER_H_
