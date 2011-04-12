/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2010 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Pipeline.h"

#include "BlobList.h"
#include "Camera.h"


Pipeline::Pipeline( TiXmlElement* _config ) {
	if (!_config) throw std::runtime_error( "Configuration file empty or not found." );
	createFilter( _config, 0 );
	Filter* last = 0;
	// FIXME: this is rather ugly...
	for (std::vector<Filter*>::reverse_iterator filter = rbegin(); filter != rend(); filter++) {
		// bloblist can use previous bloblist as parent blobs
		if (dynamic_cast<   BlobList*>(*filter) != 0) { (*filter)->link(last); last = *filter; }
		// background subtraction needs a forward link to the final output of the chain
		if (dynamic_cast<BGSubFilter*>(*filter) != 0)   (*filter)->link(last); 
	}
}

void Pipeline::createFilter( TiXmlElement* config, Filter* parent ) {

	std::string type = config->Value();
	Filter* filter = 0;

	if (type ==        "Camera") filter = new        Camera( config, parent );
	if (type ==    "BlobFilter") filter = new      BlobList( config, parent );
	if (type ==    "FlipFilter") filter = new    FlipFilter( config, parent );
	if (type ==   "SplitFilter") filter = new   SplitFilter( config, parent );
	if (type ==   "BGSubFilter") filter = new   BGSubFilter( config, parent );
	if (type ==  "ThreshFilter") filter = new  ThreshFilter( config, parent );
	if (type == "SpeckleFilter") filter = new SpeckleFilter( config, parent );
	if (type == "LowpassFilter") filter = new LowpassFilter( config, parent );
	if (type ==    "AreaFilter") filter = new    AreaFilter( config, parent );

	if (filter) push_back( filter );

	for ( TiXmlElement* child = config->FirstChildElement(); child != 0; child = child->NextSiblingElement() )
		createFilter( child, filter );
}


Pipeline::~Pipeline() {
	for (std::vector<Filter*>::iterator filter = begin(); filter != end(); filter++)
		delete *filter;
}


int Pipeline::process() {
	for (std::vector<Filter*>::iterator filter = begin(); filter != end(); filter++) {
		int res =  (*filter)->process();
		if (res != 0) return res;
	}
	return 0;
}

void Pipeline::reset() {
	for (std::vector<Filter*>::iterator filter = begin(); filter != end(); filter++)
		(*filter)->reset();
}

