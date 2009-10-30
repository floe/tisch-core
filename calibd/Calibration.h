/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#include <iostream>
#include <vector>

#include "BasicBlob.h"


inline void undistort( ::Vector& vec, ::Vector scale, ::Vector delta, const double coeff[4] ) {

		// normalize vector
		::Vector temp = vec;
		temp.x /= scale.x;
		temp.y /= scale.y;
		temp = temp - delta;

		// perform undistortion
		double len = temp.length();
		temp = temp * ( 1.0/ ( coeff[0] + coeff[1]*len + coeff[2]*len*len + coeff[3]*len*len*len ) );

		// scale back to original
		temp = temp + delta;
		temp.x *= scale.x;
		temp.y *= scale.y;
		vec = temp;
}


class Calibration {

	friend std::istream& operator>>( std::istream& s, Calibration& c );
	friend std::ostream& operator<<( std::ostream& s, Calibration& c );

	public:

		Calibration( int _verbose = 0 );

		void reset( );

		void load( );
		void save( );

		void backup ( );
		void restore( );
		
		void apply( BasicBlob& blob ) const;
		void apply( ::Vector& vec, int do_radial = 1 ) const;

		void calculate( std::vector< ::Vector >& image, std::vector< ::Vector >& screen, unsigned int homography_corrs = 0 );

		void set( double _coeff[4], ::Vector  _delta, ::Vector  _scale );
		void get( double _coeff[4], ::Vector& _delta, ::Vector& _scale );

	private:

		int verbose;
		double mat[9];
		::Vector delta;
		::Vector scale;
		double coeff[4];
		std::string name;
};

std::istream& operator>>( std::istream& s, Calibration& c );
std::ostream& operator<<( std::ostream& s, Calibration& c );

#endif // _CALIBRATION_H_

