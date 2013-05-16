/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MARKERTRACKERFILTER_H_
#define _MARKERTRACKERFILTER_H_

#include "Filter.h"

#ifdef HAS_UBITRACK
class MarkerTrackerFilter: public Filter {
	public:
		MarkerTrackerFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual int process();
		virtual void draw( GLUTWindow* win );
		virtual ~MarkerTrackerFilter();

		// Configurator
		virtual const char* getOptionName(int option);
		virtual double getOptionValue(int option);
		virtual void modifyOptionValue(double delta, bool overwrite);
		
		virtual TiXmlElement* getXMLRepresentation();
	protected:
		// variables to read from XML
		int int_mt_enabled;
		int int_mt_showMarker;
		
		// variables for further use
		bool mt_enabled;
		bool mt_showMarker;
		MarkerTracker* mMarkerTracker;
		std::vector<Ubitrack::Vision::SimpleMarkerInfo>* detectedMarkers;

};
#endif // HAS_FREENECT

#endif // _MARKERTRACKERFILTER_H_
