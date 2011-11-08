/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Calibration.h"
#include <stdlib.h>

int main() {

	double xres = 1400;
	double yres = 1050;

	std::vector<Vector> in;
	std::vector<Vector> out;

	in.push_back(Vector( 548.561, 429.882, 0 ));
	in.push_back(Vector( 83.4629, 426.518, 0 ));
	in.push_back(Vector( 94.3494, 48.1733, 0 ));
	in.push_back(Vector( 576.412, 73.7866, 0 ));

	in.push_back(Vector( 317.162, 466.158, 0 ));
	in.push_back(Vector(  67.684, 228.27,  0 ));
	in.push_back(Vector( 329.804, 35.7499, 0 ));
	in.push_back(Vector( 574.366, 249.801, 0 ));

	// four corners: determine homography
	out.push_back( Vector(      20,      30 ) );
	out.push_back( Vector( xres-20,      30 ) );
	out.push_back( Vector( xres-20, yres-30 ) );
	out.push_back( Vector(      20, yres-30 ) );

	// four pooutts midway between corners: determoute radial distortion
	out.push_back( Vector(  xres/2,      30 ) );
	out.push_back( Vector( xres-20,  yres/2 ) );
	out.push_back( Vector(  xres/2, yres-30 ) );
	out.push_back( Vector(      20,  yres/2 ) );

	for (int i = 0; i < 8; i++)
		std::cout << " in: " <<  in[i] << std::endl;
	std::cout << std::endl;
	for (int i = 0; i < 8; i++) 
		std::cout << "out: " << out[i] << std::endl;

	Calibration cal;
	cal.calculate(in,out,4);

	Calibration inv;
	inv.calculate(out,in,4);

	std::cout << "\n\n" << cal << "\n\n" << std::endl;

	Vector exp[8];
	Vector delta(320,240,0);

	for (int i = 4; i < 8; i++) {
		exp[i] = out[i];
		inv.apply( exp[i] );
		std::cout << "img" << i-4 << " = [ " <<  in[i]-delta << " ]" << std::endl;
		std::cout << "exp" << i-4 << " = [ " << exp[i]-delta << " ]" << std::endl;
	}
}

