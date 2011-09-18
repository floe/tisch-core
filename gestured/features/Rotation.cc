/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Rotation.h"
#include "Factory.h"

Rotation::Rotation( unsigned int tf ): Feature<double>( tf ) { }
Rotation::~Rotation() { }

void Rotation::load( InputState& state ) { }

RegisterFeature( Rotation );

