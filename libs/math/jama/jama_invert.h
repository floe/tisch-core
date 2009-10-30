#ifndef JAMA_INVERT_H
#define JAMA_INVERT_H

#include "tnt_array1d.h"
#include "tnt_array2d.h"
#include "jama_lu.h"

#include <assert.h>

namespace JAMA
{

using namespace TNT;

template<class T>
Array2D<T> invert(const Array2D<T> &M)
{
	// square matrices only please
	assert( M.dim1() == M.dim2() );

	// solve for inverse with LU decomposition
	LU<T> lu( M );

	// create identity matrix
	Array2D<T> id( M.dim1(), M.dim2(), (T)0.0 );
	for ( int i = 0; i < M.dim1(); i++ ) id[i][i] = 1.0;

	// solves A * A_inv = Identity
	return lu.solve(id);
}

template<class T>
Array2D<T> transpose(const Array2D<T> &M)
{
	Array2D<T> tran( M.dim2(), M.dim1() );
	for ( int r = 0; r < M.dim1(); ++r )
		for ( int c = 0; c < M.dim2(); ++c )
			tran[c][r] = M[r][c];
	return tran;
}

}
// namespace JAMA

#endif
// JAMA_INVERT_H

