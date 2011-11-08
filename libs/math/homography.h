/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

// homography calculation - based on matlab example
// by Peter Kovesi at http://www.csse.uwa.edu.au/~pk

#include "normalize.h"

#include <jama_svd.h>
#include <jama_invert.h>


#ifdef USE_LAPACK

	#include <tnt_fortran_array2d.h>

	extern "C" {
		#include "f2c.h"
		#include "clapack.h"
	}

	template<class FloatType> TNT::Array2D<FloatType> lapack_svd( TNT::Array2D<FloatType> A ) {

		TNT::Fortran_Array2D<FloatType> A2( A.dim1(), A.dim2() );

		for (int i = 0; i < A.dim1(); i++)
			for (int j = 0; j < A.dim2(); j++)
				A2(i+1,j+1) = A[i][j];

		TNT::Fortran_Array1D<FloatType> s( min(A.dim1(),A.dim2()) );

		TNT::Fortran_Array2D<FloatType> u( 9,9 );
		TNT::Fortran_Array2D<FloatType> v( 9,9, 0.0 );
		TNT::Fortran_Array1D<FloatType> work( 100 );

		logical1 nl='N';
		logical1 al='A';

		integer m = A.dim1();
		integer n = A.dim2();

		integer lda = max(1,m);
		integer ldu = max(1,m);
		integer ldv = max(1,n);

		integer lwork = 100;
		integer info = 0;

		dgesvd_( &nl, &al, &m, &n, &A2(1,1), &lda, &s(1), &u(1,1), &ldu, &v(1,1), &ldv, &work(1), &lwork, &info );

		TNT::Array2D<FloatType> V(9,9);

		for (int i = 0; i < V.dim1(); i++)
			for (int j = 0; j < V.dim2(); j++)
				V[i][j] = v(i+1,j+1);

		return V;
	}

#endif


template<class FloatType> void homography2d( TNT::Array2D<FloatType> x1, TNT::Array2D<FloatType> x2, TNT::Array2D<FloatType> H ) {

	int count = x1.dim1();
	assert( x2.dim1() == count );

	assert( x1.dim2() == 3 );
	assert( x2.dim2() == 3 );

	assert( H.dim1() == 3 );
	assert( H.dim2() == 3 );

	TNT::Array2D<FloatType> x1n(count,3);
	TNT::Array2D<FloatType> x2n(count,3);

	TNT::Array2D<FloatType> T1(3,3);
	TNT::Array2D<FloatType> T2(3,3);

	normalize2d( x1, T1, x1n );
	normalize2d( x2, T2, x2n );

	TNT::Array2D<FloatType> A( 3*count, 9, 0.0 );

	TNT::Array1D<FloatType> O( 3, 0.0 );

	for (int n = 0; n < count; n++) {

		TNT::Array1D<FloatType> X(3); for (int i = 0; i < 3; i++) X[i] = x1[n][i];

		FloatType x = x2[n][0];
		FloatType y = x2[n][1];
		FloatType w = x2[n][2];

		A[3*n+0][0] =     0.0; A[3*n+0][3] = -w*X[0]; A[3*n+0][6] =  y*X[0];
		A[3*n+0][1] =     0.0; A[3*n+0][4] = -w*X[1]; A[3*n+0][7] =  y*X[1];
		A[3*n+0][2] =     0.0; A[3*n+0][5] = -w*X[2]; A[3*n+0][8] =  y*X[2];

		A[3*n+1][0] =  w*X[0]; A[3*n+1][3] =     0.0; A[3*n+1][6] = -x*X[0];
		A[3*n+1][1] =  w*X[1]; A[3*n+1][4] =     0.0; A[3*n+1][7] = -x*X[1];
		A[3*n+1][2] =  w*X[2]; A[3*n+1][5] =     0.0; A[3*n+1][8] = -x*X[2];

		A[3*n+2][0] = -y*X[0]; A[3*n+2][3] =  x*X[0]; A[3*n+2][6] =     0.0;
		A[3*n+2][1] = -y*X[1]; A[3*n+2][4] =  x*X[1]; A[3*n+2][7] =     0.0;
		A[3*n+2][2] = -y*X[2]; A[3*n+2][5] =  x*X[2]; A[3*n+2][8] =     0.0;

	}

	TNT::Array2D<FloatType> V;

	#ifndef USE_LAPACK

		JAMA::SVD<FloatType> svd(A);
		svd.getV(V);

		H[0][0] = V[0][8];
		H[0][1] = V[1][8];
		H[0][2] = V[2][8];

		H[1][0] = V[3][8];
		H[1][1] = V[4][8];
		H[1][2] = V[5][8];

		H[2][0] = V[6][8];
		H[2][1] = V[7][8];
		H[2][2] = V[8][8];
	
	#else

		V = lapack_svd(A);

		H[0][0] = V[8][0];
		H[0][1] = V[8][1];
		H[0][2] = V[8][2];

		H[1][0] = V[8][3];
		H[1][1] = V[8][4];
		H[1][2] = V[8][5];

		H[2][0] = V[8][6];
		H[2][1] = V[8][7];
		H[2][2] = V[8][8];

	#endif

	TNT::Array2D<FloatType> T2inv = JAMA::invert( T2 );
	TNT::Array2D<FloatType> tmp1 = matmult( T2inv, H );
	TNT::Array2D<FloatType> tmp2 = matmult( tmp1, T1 );

	H = tmp2;
}

