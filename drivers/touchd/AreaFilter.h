/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _AREAFILTER_H_
#define _AREAFILTER_H_

#include "Filter.h"

class AreaFilter: public Filter {
	public:
		AreaFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		
		virtual int process();
		virtual void reset(int initialReset);

		virtual void processMouseButton(int button, int state, int x, int y);
		void generateEdgepoints( std::vector<Point*> cornerpoints );

		virtual void draw( GLUTWindow* win, int show_image );
		const char* name() const { return "AreaFilter"; }

		virtual TiXmlElement* getXMLRepresentation();
		TiXmlElement* getXMLofAreas();

		void loadFilterOptions(TiXmlElement* OptionSubtree);
		int createFilterAreaFromConfig(TiXmlElement* PolygonsOfAreaFilter, bool debug);

	protected:
		int enabled;
		int resetOnInit;
		bool updated;
		std::vector<int> edgepoints;
		std::vector<std::vector<Point*> > cornerpointvector;
};

#endif // _AREAFILTER_H_
