/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BGSUBFILTER_H_
#define _BGSUBFILTER_H_

#include "Filter.h"

class BGSubFilter: public Filter {
	public:
		BGSubFilter( TiXmlElement* _config = 0, Filter* _input = 0 );
		virtual ~BGSubFilter();
		virtual int process();
		virtual void reset(int initialReset);
		virtual void link( Filter* _mask );
		// Configurator
		virtual const char* getOptionName(int option);
		virtual double getOptionValue(int option);
		virtual void modifyOptionValue(double delta, bool overwrite);
		virtual TiXmlElement* getXMLRepresentation();
		int getBGSubFilterID();
		TiXmlElement* getXMLofBackground(int BGSubFilterID, std::string pathToSaveBackgroundIMG);
		void loadFilterOptions(TiXmlElement* OptionSubtree, bool debug);
		int loadPGMImageFromFile(std::string filename, bool debug);
	protected:
		ShortImage* background;
		Filter* mask;
		int invert, adaptive, storeBGImg;
		int BGSubFilterID; 

};

#endif // _BGSUBFILTER_H_
