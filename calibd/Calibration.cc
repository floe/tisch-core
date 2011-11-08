/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>

#include <stdexcept>
#include <fstream>
#include <string>

#include <homography.h>
#include <tnt_array2d_utils.h>

#include "Calibration.h"


Calibration::Calibration( int v ): verbose(v) {

#ifndef _MSC_VER
	char* homedir = getenv( "HOME" );
	if (!homedir) homedir = "/tmp";
	name = std::string( homedir ) + "/.tisch.calib";
#else
	char* homedir = getenv( "TEMP" );
	if (!homedir) homedir = "c:\\windows\\temp";
	name = std::string( homedir ) + "\\.tisch.calib";
#endif

	reset();

	scale = ::Vector(   640,   640, 0 );
	delta = ::Vector(   0.5, 0.375, 0 );

	coeff[0] = 1.0;
	coeff[1] = 0.0;
	coeff[2] = 0.0;
	coeff[3] = 0.0;
	coeff[4] = 0.0;
}

void Calibration::reset( ) {
	mat[0] = 1.0; mat[1] = 0.0; mat[2] = 0.0;
	mat[3] = 0.0; mat[4] = 1.0; mat[5] = 0.0;
	mat[6] = 0.0; mat[7] = 0.0; mat[8] = 1.0;
}


void Calibration::backup( ) {
#ifdef _MSC_VER
	std::string cmd( "copy " + name + " " + name + ".bak" );
#else
	std::string cmd( "cp " + name + " " + name + ".bak" );
#endif
	if ( system( cmd.c_str() ) != 0 ) throw std::runtime_error("Backup of calibration failed.");
}

void Calibration::restore( ) {
#ifdef _MSC_VER
	std::string cmd( "copy " + name + ".bak " + name );
#else
	std::string cmd( "cp " + name + ".bak " + name );
#endif
	if ( system( cmd.c_str() ) != 0 ) throw std::runtime_error("Restore of calibration failed.");
}


void Calibration::load( ) {

	std::ifstream cf( name.c_str() );

	if ( cf.fail() ) save(); // throw std::runtime_error("opening" + name + "failed.");

	cf >> (*this);
	cf.close();

	if (verbose) std::cout << "Calibration matrix loaded:\n" << (*this) << std::flush;
}


void Calibration::save( ) {

  std::ofstream cf( name.c_str(), std::ios::trunc );

  if ( cf.fail() ) throw std::runtime_error("opening " + name + " failed.");

	cf << (*this);
  cf.close();

	if (verbose) std::cout << "Calibration matrix stored:\n" << (*this) << std::flush;
}


void Calibration::apply( BasicBlob& blob ) const {

	apply( blob.pos  );
	apply( blob.peak );

}

void Calibration::apply( ::Vector& vec, int do_radial ) const {

	// apply radial undistortion?
	if (do_radial) undistort( vec, scale, delta, coeff );

	// multiply with matrix
	double x = vec.x;
	double y = vec.y;

	double tx = x*mat[0] + y*mat[1] + mat[2];
	double ty = x*mat[3] + y*mat[4] + mat[5];
	double tz = x*mat[6] + y*mat[7] + mat[8];

	// convert back to homogeneous coordinates
	vec.x = tx/tz;
	vec.y = ty/tz;
}


void Calibration::calculate( std::vector< ::Vector >& image, std::vector< ::Vector >& screen, unsigned int homography_corrs ) {

	typedef long double myfloat;

	if (!homography_corrs) homography_corrs = image.size();
	unsigned int count = homography_corrs;

	if ( count < 4 ) 
		throw std::runtime_error("Invalid number of correspondences.");

	TNT::Array2D<myfloat> src(count,3);
	TNT::Array2D<myfloat> dst(count,3);

	TNT::Array2D<myfloat> h(3,3,0.0);

	// reset matrix
	reset();

	if (verbose) std::cout << "current parameters: " << *this << std::endl;

	// assume coordinates are already normalized
	for (unsigned int i = 0; i < count; i++) {
		::Vector img_tmp = image[i];
		apply( img_tmp );
		if (verbose) {
			std::cout << "screen coordinates: " << screen[i] << std::endl;
			std::cout << "image coordinates:  " <<  image[i] << std::endl;
			std::cout << "undistorted image:  " <<   img_tmp << std::endl;
		}
		src[i][0] = img_tmp.x;
		src[i][1] = img_tmp.y;
		src[i][2] = 1.0; //image[i].z;
		dst[i][0] = screen[i].x;
		dst[i][1] = screen[i].y;
		dst[i][2] = 1.0; //screen[i].z;
	}

	homography2d( src, dst, h );

	TNT::Array2D<myfloat> ht( 3,3, h[2][2] );
	h /= ht;

	for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) mat[3*i+j] = h[i][j];

	// check whether radial distortion should also be estimated
	if (homography_corrs == image.size()) return;
}

void Calibration::set( double _coeff[5], ::Vector _delta, ::Vector _scale ) {
	for (int i = 0; i < 5; i++) coeff[i] = _coeff[i];
	delta = _delta;
	scale = _scale;
}

void Calibration::get( double _coeff[5], ::Vector& _delta, ::Vector& _scale ) {
	for (int i = 0; i < 5; i++) _coeff[i] = coeff[i];
	_delta = delta;
	_scale = scale;
}


std::istream& operator>>( std::istream& s, Calibration& c ) {
	s >> c.mat[0] >> c.mat[1] >> c.mat[2];
	s >> c.mat[3] >> c.mat[4] >> c.mat[5];
	s >> c.mat[6] >> c.mat[7] >> c.mat[8];
	s >> c.coeff[0] >> c.coeff[1] >> c.coeff[2] >> c.coeff[3];
	s >> c.scale;
	s >> c.delta;
	return s;
}

std::ostream& operator<<( std::ostream& s, Calibration& c ) {
	s << c.mat[0] << " " << c.mat[1] << " " << c.mat[2] << std::endl;
	s << c.mat[3] << " " << c.mat[4] << " " << c.mat[5] << std::endl;
	s << c.mat[6] << " " << c.mat[7] << " " << c.mat[8] << std::endl;
	s << c.coeff[0] << " " << c.coeff[1] << " " << c.coeff[2] << " " << c.coeff[3] << std::endl;
	s << c.scale << " " << std::endl;
	s << c.delta << " " << std::endl;
	return s;
}

