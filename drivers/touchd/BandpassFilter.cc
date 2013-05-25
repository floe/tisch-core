/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "BandpassFilter.h"

BandpassFilter::BandpassFilter( TiXmlElement* _config, Filter* _input ):
	Filter( _config, _input, FILTER_TYPE_BASIC)
{
	createOption( "InnerRadius",  8, 4, 64 );
	createOption( "OuterRadius", 16, 4, 64 );
}

int BandpassFilter::process() {
	int inner = options["InnerRadius"].get();
	int outer = options["OuterRadius"].get();
	if (image) input->getImage()->bandpass( *image, outer, inner );
	//else input->getShortImage()->lowpass( *shortimage, range, mode );
	return 0;
}

