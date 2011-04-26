/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Scale.h"
#include "Factory.h"

Scale::Scale( int tf ): Feature<double>( tf ) { }
Scale::~Scale() { }

void Scale::load( InputState& state ) { }

RegisterFeature( Scale );

