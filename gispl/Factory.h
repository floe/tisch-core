/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "Feature.h"

typedef FeatureBase* (*FeatureConstructor)();

TISCH_SHARED std::map<std::string,FeatureConstructor>& g_factory();

template< class Product > class TISCH_SHARED FeatureMachine {

	public:

		FeatureMachine( std::string name ) { g_factory()[ name ] = construct; }

		static FeatureBase* construct() { return new Product(); }

};

#define RegisterFeature(F) FeatureMachine< F > F##Factory( (new F)->name() )

TISCH_SHARED FeatureBase* createFeature( const std::string& name );

#endif // _FACTORY_H_

