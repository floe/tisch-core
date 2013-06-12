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
		const char* name() const { return "BGSubFilter"; }

		virtual TiXmlElement* getXMLRepresentation();
		TiXmlElement* getXMLofBackground(int BGSubFilterID, std::string pathToSaveBackgroundIMG);

		void loadFilterOptions( TiXmlElement* OptionSubtree );
		int loadPGMImageFromFile( std::string filename );

	protected:
		ShortImage* background;
		Filter* mask;
		int invert, adaptive, storeBGImg;
		int resetOnInit;
};

#endif // _BGSUBFILTER_H_
