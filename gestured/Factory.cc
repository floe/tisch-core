/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Factory.h"

std::map<std::string,FeatureConstructor>& g_factory() {
	static std::map<std::string,FeatureConstructor>* s_map = new std::map<std::string,FeatureConstructor>();
	return *s_map;
}

FeatureBase* createFeature( const std::string& name ) {
	std::map<std::string,FeatureConstructor>::iterator mach = g_factory().find( name );
	if (mach == g_factory().end() ) { 
		std::cerr << "Warning: unknown feature '" << name << "' requested from Factory." << std::endl;
		return 0;
	}
	return (mach->second)();
}

