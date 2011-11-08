/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

// normalization of homogeneous coordinates - based on matlab
// example by Peter Kovesi at http://www.csse.uwa.edu.au/~pk

#include <math.h>
#include <assert.h>

#include <tnt_array2d.h>

#define sqr(x) x*x

//using namespace TNT;


template<class FloatType> void normalize2d( TNT::Array2D<FloatType> pts, TNT::Array2D<FloatType> T, TNT::Array2D<FloatType> newpts ) {

	// let's assume that pts are already normalized wrt 3rd coordinate (=1)
	int count = pts.dim1();
	assert( newpts.dim1() == count );

	assert( newpts.dim2() == 3 );
	assert( pts.dim2() == 3 );

	assert( T.dim1() == 3 );
	assert( T.dim2() == 3 );

	// calculate the centroid
	FloatType c0 = 0;
	FloatType c1 = 0;

	for (int i = 0; i < count; i++) {
		c0 += pts[i][0];
		c1 += pts[i][1];
	}

	c0 /= (FloatType)count;
	c1 /= (FloatType)count;

	// shift origin to centroid
	for (int i = 0; i < count; i++) {
		newpts[i][0] = pts[i][0] - c0;
		newpts[i][1] = pts[i][1] - c1;
	}

	// get mean distance from origin
	FloatType meandist = 0;

	for (int i = 0; i < count; i++)
		meandist += sqrt( sqr(newpts[i][0]) + sqr(newpts[i][1]) );

	meandist /= (FloatType)count;

	// rescale to distance sqrt(2)
	FloatType scale = sqrt(2.0) / meandist;

	T[0][0] = scale; T[0][1] =   0.0; T[0][2] = -scale*c0;
	T[1][0] =   0.0; T[1][1] = scale; T[1][2] = -scale*c1;
	T[2][0] =   0.0; T[1][1] =   0.0; T[1][2] =       1.0;

	// matrix-multiply all vectors
	for (int i = 0; i < count; i++)
		for (int j = 0; j < 3; j++) 
			newpts[i][j] = T[j][0]*pts[i][0] + T[j][1]*pts[i][1] + T[j][2]*pts[i][2];

}


/*template<class FloatType> void normalize( FloatType pts[][3], int count, FloatType T[3][3], FloatType newpts[][3] ) {

	// let's assume that pts are already normalized wrt 3rd coordinate (=1)

	// calculate the centroid
	FloatType c0 = 0;
	FloatType c1 = 0;

	for (int i = 0; i < count; i++) {
		c0 += pts[i][0];
		c1 += pts[i][1];
	}

	c0 /= (FloatType)count;
	c1 /= (FloatType)count;

	// shift origin to centroid
	for (int i = 0; i < count; i++) {
		newpts[i][0] = pts[i][0] - c0;
		newpts[i][1] = pts[i][1] - c1;
	}

	// get mean distance from origin
	FloatType meandist = 0;

	for (int i = 0; i < count; i++)
		meandist += sqrt( sqr(newpts[i][0]) + sqr(newpts[i][1]) );

	meandist /= (FloatType)count;

	// rescale to distance sqrt(2)
	FloatType scale = sqrt(2.0) / meandist;

	T[0][0] = scale; T[0][1] =   0.0; T[0][2] = -scale*c0;
	T[1][0] =   0.0; T[1][1] = scale; T[1][2] = -scale*c1;
	T[2][0] =   0.0; T[1][1] =   0.0; T[1][2] =       1.0;

	// matrix-multiply all vectors
	for (int i = 0; i < count; i++)
		for (int j = 0; j < 3; j++) 
			newpts[i][j] = T[j][0]*pts[i][0] + T[j][1]*pts[i][1] + T[j][2]*pts[i][2];

}*/

