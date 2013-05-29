/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "ThreshFilter.h"

// TODO: use result from bgsub filter for threshold adjustment
ThreshFilter::ThreshFilter( TiXmlElement* _config, Filter* _input ):
	Filter( _config, _input, FILTER_TYPE_GREY )
{
	createOption( "LowerThreshold", 128, 0, 255 );
	createOption( "UpperThreshold", 255, 0, 255 );
}

int ThreshFilter::process() {
	int threshold_min = options["LowerThreshold"]->get();
	int threshold_max = options["UpperThreshold"]->get();
	if (image) input->getImage()->threshold( threshold_min, *image, threshold_max );
	else input->getShortImage()->threshold( threshold_min << 5, *shortimage, threshold_max << 5 );
	return 0;
}

