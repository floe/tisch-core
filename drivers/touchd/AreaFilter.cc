/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "AreaFilter.h"

AreaFilter::AreaFilter( TiXmlElement* _config, Filter* _input ): Filter( _config, _input ) {
	checkImage();
	enabled = 0;
	updated = true;
	resetOnInit = 1; // is set to 0 if polygons where read from config to edgepoint vector
	AreaFilterID = -1;
	config->QueryIntAttribute( "AreaFilterID", &AreaFilterID );
	config->QueryIntAttribute( "Enabled", &enabled );
	// setting variables for Configurator
	countOfOptions = 1; // quantity of variables that can be manipulated
}

int AreaFilter::process() {
	rgbimage = input->getRGBImage(); // get pointer from previous filter, do nothing
	if(enabled)
		if(useIntensityImage) input->getImage()->areamask( *image, edgepoints );
		else input->getShortImage()->areamask( *shortimage, edgepoints );
	else
		if(useIntensityImage) *image = *(input->getImage());
		else *shortimage = *(input->getShortImage());
	return 0;
}

void AreaFilter::processMouseButton( int button, int state, int x, int y )
{
	//add a cornerpoint to the newest polygon
	if(button == 0 && state == 0)
	{
		if(updated) 
		{
			cornerpointvector.push_back(std::vector<Point*>());
			updated = false;
		}
		Point* a = new Point();
		a->x = x;
		a->y = y;
		(*(cornerpointvector.end() - 1)).push_back(a);
	}

	//update polygons
	if(button == 2 && state == 0 && !updated)
	{
		updated = true;
		edgepoints.clear();

		if(cornerpointvector.empty()) return;

		for(std::vector<std::vector<Point*> >::iterator it = cornerpointvector.begin(); it != cornerpointvector.end(); it++)
		{
			if((*it).empty()) continue;
			if(*((*it).begin()) != *((*it).end()-1)) (*it).push_back(*(*it).begin()); //copies the first point of a polygon to the end of its pointvector
			generateEdgepoints(*it);
		}
		std::sort(edgepoints.begin(), edgepoints.end()); // sort the list so that the areamask is applied to the right pixelareas
	}
}

//generates all edgepoints between subsequent cornerpoints
void AreaFilter::generateEdgepoints( std::vector<Point*> cornerpoints )
{
	for(std::vector<Point*>::iterator it = cornerpoints.begin(); it != cornerpoints.end()-1; it++)
	{	
		int max = fabs((double)((*it)->y - (*(it + 1))->y));
		if( max != 0)
			for(int i = 0; i < max; i++)
			{
				double helpx = (*it)->x + ( i * ((*(it + 1))->x - (*it)->x) / max );
				double helpy = (((*(it + 1))->y - (*it)->y) < 0 ? -1 : 1) * (i + 0.5) + (*it)->y;
				edgepoints.push_back((helpy-0.5)*image->getWidth() + helpx); //converts coordinates into data indices
			}
	}
}

void AreaFilter::reset(int initialReset)
{
	if( (initialReset == 1 && resetOnInit == 1) || initialReset == 0 ) {
		edgepoints.clear();
		cornerpointvector.clear();
		updated = true;
	}
}

const char* AreaFilter::getOptionName(int option) {
	const char* OptionName = "";

	switch(option) {
	case 0:
		OptionName = "Enabled";
		break;
	default:
		// leave OptionName empty
		break;
	}

	return OptionName;
}

double AreaFilter::getOptionValue(int option) {
	double OptionValue = -1.0;

	switch(option) {
	case 0:
		OptionValue = enabled;
		break;
	default:
		// leave OptionValue = -1.0
		break;
	}

	return OptionValue;
}

void AreaFilter::modifyOptionValue(double delta, bool overwrite) {
	switch(toggle) {
	case 0: // enabled is a boolean value
		if(overwrite) {
			enabled = (delta == 0 ? 0 : (delta == 1 ? 1 : enabled));
		} else {
			enabled += delta;
			enabled = (enabled < 0) ? 0 : (enabled > 1) ? 1 : enabled;
		}
		break;
	}
}

void AreaFilter::draw( GLUTWindow* win )
{ 
	if(useIntensityImage)
		win->show( *image, 0, 0 );
	else if( displayRGBImage )
		win->show( *rgbimage, 0, 0 );
	else
		win->show( *shortimage, 0, 0 );

	glColor4f(1,0,0,1);
	for(std::vector<std::vector<Point*> >::iterator it = cornerpointvector.begin(); it != cornerpointvector.end(); it++)
		win->drawPolygon( *it, 1, image->getHeight() );
}

TiXmlElement* AreaFilter::getXMLRepresentation() {
	TiXmlElement* XMLNode = new TiXmlElement( "AreaFilter" );
	
	XMLNode->SetAttribute( "AreaFilterID" , AreaFilterID );
	XMLNode->SetAttribute( "Enabled" , enabled );
	
	return XMLNode;
}

int AreaFilter::getAreaFilterID() {
	return AreaFilterID;
}

TiXmlElement* AreaFilter::getXMLofAreas(int AreaFilterID) {
	
	int polygoncounter = 0;

	TiXmlElement* polygonsOfAreaFilter = new TiXmlElement( "AreaFilter" );
	polygonsOfAreaFilter->SetAttribute( "AreaFilterID" , AreaFilterID );

	// no areas are selected
	if(cornerpointvector.empty())
		return 0;

	for(std::vector<std::vector<Point*> >::iterator iter_polygons = cornerpointvector.begin(); iter_polygons != cornerpointvector.end(); iter_polygons++) {
		// iterate through all polygons

		// ignore polygons with less than 3 edges
		if((*iter_polygons).size() < 3) {
			continue;
		}

		TiXmlElement* polygon = new TiXmlElement( "Polygon" );
		polygon->SetAttribute("number", polygoncounter );

		for(std::vector<Point*>::iterator iter_points = (*iter_polygons).begin(); iter_points != (*iter_polygons).end(); iter_points++) {
			// iterate through all points of current polygon

			// create XML Node and store point values
			TiXmlElement* point = new TiXmlElement( "Point" );
			point->SetAttribute("x", (*iter_points)->x);
			point->SetAttribute("y", (*iter_points)->y);

			// store XML Node in tree
			polygon->LinkEndChild(point);

		} // end iter_points

		polygonsOfAreaFilter->LinkEndChild(polygon);
		polygoncounter++;
		
	} // end iter_polygons

	if(polygonsOfAreaFilter->NoChildren())
		return 0;
	return polygonsOfAreaFilter;
}

void AreaFilter::loadFilterOptions(TiXmlElement* OptionSubtree, bool debug) {
	
	// check OptionSubtree to find settings for current AreaFilter ...
	std::cout << "reading stored areas for AreaFilter from config ... ";
	if(debug)
		 std::cout << std::endl;

	TiXmlElement* filterOption = OptionSubtree->FirstChildElement();
	do {
		std::string type = filterOption->Value();
		if(type == "AreaFilter") {
			// current Options are for an AreaFilter
			int filterID = -1;
			filterOption->QueryIntAttribute( "AreaFilterID" , &filterID);
			if( filterID == AreaFilterID && filterID != -1) {
				// settings are for current AreaFilter

				// filterOption has AreaFilter Subtree -> children are Polygons
				if(debug)
					std::cout << "AreaFilterID: " << AreaFilterID << std::endl;
				
				resetOnInit = createFilterAreaFromConfig(filterOption, debug);
				break;
			}
		}
	} while((filterOption = filterOption->NextSiblingElement()));
	
	std::cout << "done" << std::endl;

}

/*
* returns 0 if edgepoint vector was filled with polygons from config
* else returns 1;
*/
int AreaFilter::createFilterAreaFromConfig(TiXmlElement* PolygonsOfAreaFilter, bool debug) {
	
	// reset cornerpointvector for this areafilter
	cornerpointvector.clear();
	edgepoints.clear();

	// first polygon
	TiXmlElement* Polygon = PolygonsOfAreaFilter->FirstChildElement();

	do {
		// process polygon
		TiXmlElement* Coords = Polygon->FirstChildElement();
		int polygonNr;
		Polygon->QueryIntAttribute("number", &polygonNr);
		
		if(debug)
			std::cout << "Polygon: " << polygonNr << std::endl;
		
		cornerpointvector.push_back(std::vector<Point*>());
					
		do {
			// process Points
			int x_coord;
			int y_coord;

			Coords->QueryIntAttribute("x", &x_coord);
			Coords->QueryIntAttribute("y", &y_coord);
			
			if(debug)
				std::cout << "Point: " << "x: " << x_coord << " / " << "y: " << y_coord << std::endl;
		
			Point* p = new Point();
			p->x = x_coord;
			p->y = y_coord;

			(*(cornerpointvector.end() - 1)).push_back(p);

			// get next point
		} while ((Coords = Coords->NextSiblingElement()));
		
		generateEdgepoints(*(cornerpointvector.end() - 1));
		std::sort(edgepoints.begin(), edgepoints.end());
		
		// get next polygon
	} while((Polygon = Polygon->NextSiblingElement()));

	if(debug) {
		for(std::vector<std::vector<Point*> >::iterator it = cornerpointvector.begin(); it != cornerpointvector.end(); it++) {
			for(std::vector<Point*>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++) {
				std::cout << "x: " << (*(*it2)).x << " y: " << (*it2)->y << std::endl;
			}
		}
	}

	if(edgepoints.size() > 0)
		return 0;
	return 1;
}
