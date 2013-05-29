/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "LowpassFilter.h"

LowpassFilter::LowpassFilter( TiXmlElement* _config, Filter* _input ):
	Filter( _config, _input, FILTER_TYPE_GREY )
{
	createOption( "Mode", 0, 0, 2 );
	createOption("Range", 1       );
}

int LowpassFilter::process() {
	int mode  = options["Mode" ]->get();
	int range = options["Range"]->get();
	if (image) input->getImage()->lowpass( *image, range, mode );
	else input->getShortImage()->lowpass( *shortimage, range, mode );
	return 0;
}

