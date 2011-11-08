/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#include <iostream>
#include <vector>

#include "BasicBlob.h"


inline void undistort( ::Vector& vec, ::Vector scale, ::Vector delta, const double coeff[5] ) {

		// normalize vector
		::Vector temp = vec;
		temp = temp - delta;
		temp.x /= scale.x;
		temp.y /= scale.y;

		::Vector result;

		// perform undistortion. note: according to the Matlab camera calibration toolbox
		// and the paper by Zhang, the 2nd and 4th order coefficients are most important.
		double len = temp.length();
		double p2  = len*len;
		double p4  = p2*p2;
		double p6  = p2*p4;
		result.x = temp.x * ( 1 + coeff[0]*p2 + coeff[1]*p4 + coeff[4]*p6 ) + 2*coeff[2]*temp.x*temp.y + coeff[3]*(p2+2*temp.x*temp.x);
		result.y = temp.y * ( 1 + coeff[0]*p2 + coeff[1]*p4 + coeff[4]*p6 ) + coeff[2]*(p2+2*temp.x*temp.x) + 2*coeff[3]*temp.x*temp.y;
		temp = result;

		// scale back to original
		temp.x *= scale.x;
		temp.y *= scale.y;
		temp = temp + delta;
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

		void set( double _coeff[5], ::Vector  _delta, ::Vector  _scale );
		void get( double _coeff[5], ::Vector& _delta, ::Vector& _scale );

	private:

		int verbose;
		double mat[9];
		::Vector delta;
		::Vector scale;
		double coeff[5];
		std::string name;
};

std::istream& operator>>( std::istream& s, Calibration& c );
std::ostream& operator<<( std::ostream& s, Calibration& c );

#endif // _CALIBRATION_H_

