/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _PICOPNG_H_
#define _PICOPNG_H_

#include <vector>
#include <string>

class TISCH_SHARED PNGImage {

	public:
	
		PNGImage( const std::string& file );

		unsigned char* data();

		unsigned long int width();
		unsigned long int height();

	protected:

		unsigned long int m_width;
		unsigned long int m_height;

		std::vector<unsigned char> buffer;

};

#endif // _PICOPNG_H_

