/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Pipeline.h"

#include "BlobList.h"
#include "Camera.h"

#include "AreaFilter.h"
#include "BandpassFilter.h"
#include "BGSubFilter.h"
#include "FlipFilter.h"
#include "LowpassFilter.h"
#include "SpeckleFilter.h"
#include "SplitFilter.h"
#include "ThreshFilter.h"


Pipeline::Pipeline( TiXmlElement* _config ) {
	// _config = root node of XML
	TiXmlElement* filterSubtree = _config->FirstChildElement(); // access Filter Node
	_config = filterSubtree->FirstChildElement(); // access Camera Node

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

	if (type ==         "Camera") filter = new         Camera( config, parent );
	if (type ==       "BlobList") filter = new       BlobList( config, parent );
	if (type ==     "FlipFilter") filter = new     FlipFilter( config, parent );
	if (type ==     "AreaFilter") filter = new     AreaFilter( config, parent );
	if (type ==    "SplitFilter") filter = new    SplitFilter( config, parent );
	if (type ==    "BGSubFilter") filter = new    BGSubFilter( config, parent );
	if (type ==   "ThreshFilter") filter = new   ThreshFilter( config, parent );
	if (type ==  "SpeckleFilter") filter = new  SpeckleFilter( config, parent );
	if (type ==  "LowpassFilter") filter = new  LowpassFilter( config, parent );
	if (type == "BandpassFilter") filter = new BandpassFilter( config, parent );

	if (type == "Options") return;

	if (filter) push_back( filter );
	else std::cout << "Warning: unknown filter element \"" << type << "\"" << std::endl;

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

void Pipeline::reset(int initialReset) {
	for (std::vector<Filter*>::iterator filter = begin(); filter != end(); filter++)
		(*filter)->reset(initialReset);
}

TiXmlElement* Pipeline::getXMLSubTree(int startIndex, Filter* parentOfRoot) {
	
	TiXmlElement* rootOfCurrentSubtree = 0;

	if(startIndex < (int)this->size()) {
		
		// get XML Node for current root of subtree
		rootOfCurrentSubtree = (*this)[startIndex]->getXMLRepresentation();
		
		// check pipe for further children of current root
		for(unsigned int i = startIndex + 1; i < this->size(); i++) {

			// get parent of current filter
			Filter* parentOfCurrent = (*this)[i]->getParent();

			// if parent of current == rootOfCurrentSubtree
			if(parentOfCurrent == (*this)[startIndex]) {
				// then current is child of root
				rootOfCurrentSubtree->LinkEndChild(getXMLSubTree(i, (*this)[i]));
			}
		}
	}

	return rootOfCurrentSubtree;
}

void Pipeline::storeXMLConfig(std::string storingTarget) {
	// store filter settings
	std::cout << "storing XML Config ... " << std::flush;

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "yes");
	doc.LinkEndChild(decl);

	// create root node
	TiXmlElement* root = new TiXmlElement( "libTISCH" );
	root->SetAttribute("version", "2.0" );

	// configuration of Filter
	TiXmlElement* FilterSubtree = new TiXmlElement( "Filter" );
	FilterSubtree->LinkEndChild(getXMLSubTree(0, 0));
	root->LinkEndChild(FilterSubtree);

	// determine the location to store BGSubFilter Images, same folder as config
	size_t found;
	found = storingTarget.find_last_of("/\\");
	std::string path = storingTarget.substr(0,found+1);

	// add root to document
	doc.LinkEndChild(root);

	// save document to file
	doc.SaveFile(storingTarget);

	std::cout << "done" << std::endl;
}
