/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Pipeline2.h"

#include "BlobList.h"
#include "Camera.h"


Pipeline2::Pipeline2( TiXmlElement* _config ) {
	createFilter( _config, 0 );
	Filter* last = 0;
	for (std::vector<Filter*>::reverse_iterator filter = rbegin(); filter != rend(); filter++) {
		// background subtraction needs a forward link to the final output of the chain
		if (dynamic_cast<SpeckleFilter*>(*filter) != 0) last = *filter; //std::cout << "speckle" << std::endl; }
		if (dynamic_cast<BGSubFilter*>(*filter) != 0) (*filter)->link(last); //std::cout << "bgsub" << std::endl; }
	}
}

void Pipeline2::createFilter( TiXmlElement* config, Filter* parent ) {

	std::string type = config->Value();
	Filter* filter = 0;

	if (type ==        "Camera") filter = new        Camera( config, parent );
	if (type ==    "BlobFilter") filter = new      BlobList( config, parent );
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

void Pipeline2::reset() {
	for (std::vector<Filter*>::iterator filter = begin(); filter != end(); filter++)
		(*filter)->reset();
}

