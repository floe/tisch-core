/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

//#define USE_LAPACK 1
#include "homography.h"
#include <tnt_array2d_utils.h>

using namespace TNT;

typedef long double myfloat;


Array2D<myfloat> t1(4,3);
Array2D<myfloat> t2(4,3);

Array2D<myfloat> h(3,3,0.0);

int main( ) {

	/*srandom(time(0));
	srand(  time(0));*/

	//while (1) {

		t1[0][0] = 0;
		t1[0][1] = 0;
		t1[0][2] = 1;

		t1[1][0] = 0;
		t1[1][1] = 2;
		t1[1][2] = 1;

		t1[2][0] = 2;
		t1[2][1] = 0;
		t1[2][2] = 1;

		t1[3][0] = 2;
		t1[3][1] = 2;
		t1[3][2] = 1;

		t2[0][0] = 0;
		t2[0][1] = 0;
		t2[0][2] = 1;

		t2[1][0] = 0;
		t2[1][1] = 1;
		t2[1][2] = 1;

		t2[2][0] = 1;
		t2[2][1] = 0;
		t2[2][2] = 1;

		t2[3][0] = 2;
		t2[3][1] = 2;
		t2[3][2] = 1;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 2; j++) {
				t1[i][j] = ((myfloat)rand()/(myfloat)RAND_MAX)*1000000;
				t2[i][j] = ((myfloat)rand()/(myfloat)RAND_MAX)*1000000;
			}
			t1[i][2] = t2[i][2] = 1;
		}

		Array2D<myfloat> t3(4,3);

		homography2d( t1, t2, h );

		std::cout << "t1: " << t1 << std::endl;
		std::cout << "t2: " << t2 << std::endl;

		std::cout << "h: " << h << std::endl;

		Array2D<myfloat> ht( 3,3, h[2][2] );
		h /= ht;

		std::cout << "h: " << h << std::endl;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 3; j++) 
				t3[i][j] = h[j][0]*t1[i][0] + h[j][1]*t1[i][1] + h[j][2]*t1[i][2];

		for (int i = 0; i < 4; i++) {
			t3[i][0] /= t3[i][2];
			t3[i][1] /= t3[i][2];
			t3[i][2]  = 1;
		}

		//std::cout << "t3 = h*t1:" << t3 << std::endl;

		std::cout << "errors: " << std::endl;

		for (int i = 0; i < 4; i++) {
			myfloat x = t2[i][0]-t3[i][0];
			myfloat y = t2[i][1]-t3[i][1];
			myfloat err = sqrt(x*x+y*y);
			std::cout << err << std::endl;
		}
		std::cout << std::endl;
}

