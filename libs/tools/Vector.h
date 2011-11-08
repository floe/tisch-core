/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <vector>
#include <math.h>

#include <nanolibc.h>

template< typename Type > class _Vector {

	public:

		_Vector( Type _x = 0, Type _y = 0, Type _z = 0 ) { x = _x; y = _y; z = _z; }

		_Vector operator-() {
			_Vector tmp;
			tmp.x = -x;
			tmp.y = -y;
			tmp.z = -z;
			return tmp;
		}

		_Vector operator+(_Vector in) {
			_Vector tmp;
			tmp.x = x + in.x;
			tmp.y = y + in.y;
			tmp.z = z + in.z;
			return tmp;
		}

		_Vector operator-(_Vector in) {
			_Vector tmp;
			tmp.x = x - in.x;
			tmp.y = y - in.y;
			tmp.z = z - in.z;
			return tmp;
		}

		_Vector operator&(_Vector in) {
			_Vector tmp;
			tmp.x = y*in.z - z*in.y;
			tmp.y = z*in.x - x*in.z;
			tmp.z = x*in.y - y*in.x;
			return tmp;
		}

		double operator*(_Vector in) const {
			double tmp;
			tmp = x*in.x + y*in.y + z*in.z;
			return tmp;
		}

		bool operator!=( const _Vector& other ) {
			return ((x != other.x) || (y != other.y) || (z != other.z));
		}

		_Vector operator*(double in) const {
			_Vector tmp;
			tmp.x = x*in;
			tmp.y = y*in;
			tmp.z = z*in;
			return tmp;
		}


		void normalize() { double tmp = length(); if (tmp == 0.0) return; x = (Type)(x/tmp); y = (Type)(y/tmp); z = (Type)(z/tmp); }

		void set( Type _x = 0, Type _y = 0, Type _z = 0 ) { x = _x; y = _y; z = _z; }

		double length() const { return sqrt((double)x*x+y*y+z*z); }

		void rotate( double angle ) {
			double tx = x *  cos(angle) + y * sin(angle);
			double ty = x * -sin(angle) + y * cos(angle);
			x = (Type)tx; y = (Type)ty;
		}

		_Vector<int> round() { return _Vector<int>( (int)::round(x), (int)::round(y), (int)::round(z) ); }

		operator const Type* () { return &x; }

		Type x,y,z;
};


template< typename Type > std::ostream& operator<< ( std::ostream& s, const _Vector<Type>& t ) {
	return s << t.x << " " << t.y << " " << t.z;
}

template< typename Type > std::istream& operator>> ( std::istream& s, _Vector<Type>& t ) {
	return s >> t.x >> t.y >> t.z;
}

template< typename Type > std::ostream& operator<< ( std::ostream& s, const std::vector< _Vector<Type> >& v ) {
	for (typename std::vector< _Vector<Type> >::const_iterator it = v.begin(); it != v.end(); it++)
		s << *it << std::endl;
	return s;
}

typedef _Vector<int> Point;
typedef _Vector<float> TexVec;
typedef _Vector<double> Vector;
typedef _Vector<unsigned int> Triangle;
typedef _Vector<unsigned char> Color;


template< typename Type > class _Quaternion: public _Vector< Type > {

	public:

		using _Vector<Type>::x;
		using _Vector<Type>::y;
		using _Vector<Type>::z;

		_Quaternion( Type _x = 0, Type _y = 0, Type _z = 0, Type _w = 1 ) { set(_x,_y,_z,_w); }

		void set( Type _x = 0, Type _y = 0, Type _z = 0, Type _w = 1 ) { x = _x; y = _y; z = _z; w = _w; }

		double length() { return sqrt(x*x+y*y+z*z+w*w); }

		void normalize() { double tmp = length(); if (tmp == 0.0) return; x = x/tmp; y = y/tmp; z = z/tmp; w = w/tmp; }

		_Quaternion operator*( _Quaternion in ) {
			_Quaternion tmp;
			tmp.w = w*in.w - x*in.x - y*in.y - z*in.z;
			tmp.x = w*in.x + x*in.w + y*in.z - z*in.y;
			tmp.y = w*in.y + y*in.w + z*in.x - x*in.z;
			tmp.z = w*in.z + z*in.w + x*in.y - y*in.x;
			tmp.normalize();
			return tmp;
		}

		void matrix( Type mat[16] ) {

			double xx = x * x;
			double xy = x * y;
			double xz = x * z;
			double xw = x * w;
			double yy = y * y;
			double yz = y * z;
			double yw = y * w;
			double zz = z * z;
			double zw = z * w;

			mat[0]  = 1 - 2 * ( yy + zz );
			mat[1]  =     2 * ( xy - zw );
			mat[2]  =     2 * ( xz + yw );
			mat[3]  = 0;

			mat[4]  =     2 * ( xy + zw );
			mat[5]  = 1 - 2 * ( xx + zz );
			mat[6]  =     2 * ( yz - xw );
			mat[7]  = 0;

			mat[8]  =     2 * ( xz - yw );
			mat[9]  =     2 * ( yz + xw );
			mat[10] = 1 - 2 * ( xx + yy );
			mat[11] = 0;

			mat[12] = 0; 
			mat[13] = 0;
			mat[14] = 0;
			mat[15] = 1;
		}

		Type w;
};



template< typename Type > std::ostream& operator<< ( std::ostream& s, const _Quaternion<Type>& q ) {
	return s << q.x << " " << q.y << " " << q.z << " " << q.w;
}

template< typename Type > std::istream& operator>> ( std::istream& s, _Quaternion<Type>& q ) {
	return s >> q.x >> q.y >> q.z >> q.w;
}

typedef _Quaternion<double> Quaternion;

#endif

