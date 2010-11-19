/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Pipeline2.h"

Pipeline2::Pipeline2( TiXmlElement* _config ) {
	createFilter( _config, 0 );
}

void Pipeline2::createFilter( TiXmlElement* config, Filter* parent ) {

	std::string type = config->Value();
	Filter* filter = 0;

	if (type ==  "SourceFilter") filter = new  SourceFilter( config, parent );
	if (type ==   "BGSubFilter") filter = new   BGSubFilter( config, parent );
	if (type ==  "ThreshFilter") filter = new  ThreshFilter( config, parent );
	if (type == "SpeckleFilter") filter = new SpeckleFilter( config, parent );

	if (filter) push_back( filter );

	for ( TiXmlElement* child = config->FirstChildElement(); child != 0; child = child->NextSiblingElement() )
		createFilter( child, filter );
}


Pipeline2::~Pipeline2() {
	for (std::vector<Filter*>::iterator filter = begin(); filter != end(); filter++)
		delete *filter;
}


void Pipeline2::process() {
	for (std::vector<Filter*>::iterator filter = begin(); filter != end(); filter++)
		(*filter)->process();
}

