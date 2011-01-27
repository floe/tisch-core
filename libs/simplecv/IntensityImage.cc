/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "IntensityImage.h"
#include "mmx.h"

#include <string.h>
#include <math.h>

#include <algorithm>
#include <fstream>
#include <string>

#include <Calibration.h>


/*inline int IntensityImage::pixelOffset(int x , int y, int channel) const { return (y*width)+x; }
inline unsigned char IntensityImage::getPixel(int x, int y, int channel) const { return data[(y*width)+x]; }
inline void IntensityImage::setPixel(int x, int y, unsigned char value, int channel) { data[(y*width)+x] = value; }*/


void IntensityImage::cross( int x, int y, int size, unsigned char color ) {
  for ( int i = x-(size/2); i <= x+(size/2); i++ ) setPixel(i,y,color);
  for ( int i = y-(size/2); i <= y+(size/2); i++ ) setPixel(x,i,color);
}

void IntensityImage::box( int x1, int y1, int x2, int y2, unsigned char color ) {
	for ( int i = x1; i <= x2; i++ ) { setPixel(i,y1,color); setPixel(i,y2,color); }
	for ( int i = y1; i <= y2; i++ ) { setPixel(x1,i,color); setPixel(x2,i,color); }
}


IntensityImage::IntensityImage( const char* path ) {

	int fwidth,fheight,fbpp;
	std::string magic,tmp;

	// open file with whitespace skipping
	std::ifstream myfile( path, std::ios::in );
	myfile >> std::skipws;

	// parse the header
	myfile >> magic;   myfile.ignore(1); if (myfile.peek() == '#') getline( myfile, tmp );
	myfile >> fwidth;  myfile.ignore(1); if (myfile.peek() == '#') getline( myfile, tmp );
	myfile >> fheight; myfile.ignore(1); if (myfile.peek() == '#') getline( myfile, tmp );
	myfile >> fbpp;

	if ((magic != "P5") || (fbpp > 255) || (fbpp < 1)) 
		throw std::runtime_error( std::string("IntensityImage: ") + std::string(path) + std::string(": no valid PGM file") );

	// init the base class
	init( fwidth, fheight, 1, 0, 0 );

	// skip one byte, read the rest
	myfile.ignore( 1 );
	myfile.read( (char*)data, size );
	myfile.close( );
}


int IntensityImage::histogram( int hg[] ) const {

	int max = 0;
	int pos = 0;

	for (int i = 0; i < 256; i++) hg[i] = 0;

	for (int i = 0; i < count; i++) hg[data[i]] += 1;

	for (int i = 0; i < 256; i++) 
		if (hg[i] > max) {
			max = hg[i];
			pos = i;
		}

	return pos;
}

int IntensityImage::intensity( ) const {
	#ifndef NOMMX
		return mmxintensity( data, count );
	#else
		long long int tmp = 0;
		for (int i = 0; i < count; i++) tmp += data[i];
		return (int)(tmp/(long long int)count);
	#endif
}


IntensityImage::IntensityImage( const IntensityImage& img, int downsample ):
	Image( (img.width)/2, (img.height)/2, 1 ) 
{
	int w1 = (img.width)/2;
	int h1 = (img.height)/2;
	for (int w = 0; w < w1; w++) for (int h = 0; h < h1; h++) {
		int tmp = 0;
		int offset = img.pixelOffset(w*2,h*2);
		tmp += img.data[offset  ];
		tmp += img.data[offset+1]; offset += img.width;
		tmp += img.data[offset  ];
		tmp += img.data[offset+1];
		setPixel(w,h,tmp/4,0);
	}
}


inline long long int   sum( int x ) { long long int t = x; return (t*(t+1))/2;         } // 1 + 2 + ... + x
inline long long int sqsum( int x ) { long long int t = x; return (t*(t+1)*(2*t+1))/6; } // 1^2 + 2^2 + ... + x^2

inline long long int   sum( int x1, int x2 ) { return   sum(x2)-  sum(x1-1); } // x1 + x1+1 + ... + x2
inline long long int sqsum( int x1, int x2 ) { return sqsum(x2)-sqsum(x1-1); } // x1^2 + (x1+1)^2 + ... + x2^2


void IntensityImage::scanSpan( int add, int dir, int x1, int x2, int y, Moments* m ) {

	// error condition:
	// scanspan: add -2, dir 1, x1 751, x2 752, y 436
	// scanspan: add 1, dir 1, x1 752, x2 751, y 437
	// scanspan: add 1, dir 1, x1 752, x2 751, y 437
	// ..... ad nauseam.
	// printf("scanspan: add %d, dir %d, x1 %d, x2 %d, y %d\n",add,dir,x1,x2,y);

	// add error checking for x1,x2 >= width
	if (x2 < x1) std::swap(x1,x2); // return
	if (x1 >= width) x1 = width-1;
	if (x2 >= width) x2 = width-1;

	if (add > 0) {

		long long int pcount = x2 - x1 + 1;
		long long int tmpsum = sum(x1,x2);
		int y0 = y-dir;

		m->m00 += pcount;
		m->m01 += pcount * y0;
		m->m02 += pcount * y0*y0;

		m->m10 += tmpsum;
		m->m20 += sqsum(x1,x2);
		m->m11 += tmpsum*y0;
	}

	if ((y < 0) || (y >= height)) return;

	register unsigned char oldcol = m->ov;
	register unsigned char newcol = m->nv;

	register int offs = width*y;
	register int x = x1;
	register int l = -1;

	// check for leaks to the left
	while ((x >= 0) && (data[offs+x] == oldcol)) { data[offs+x] = newcol; l = x--; }

	if (l != -1) {
		// leak found, see how far it extends to the right
		x = x1+1; while ((x < width) && (data[offs+x] == oldcol)) { data[offs+x] = newcol; x++; }
		scanSpan( -1, -dir, l, x-1, y-dir, m ); // FIXME: x-1 or x1-1?
	}

	// loop through normal spans
	while (x <= x2) {
		if (l != -1) scanSpan( 1, dir, l, x-1, y+dir, m );
		while ((x <= x2) && (data[offs+x] != oldcol)) x++;
		if (x <= x2) {
			l = x;
			while ((x < width) && (data[offs+x] == oldcol)) { data[offs+x] = newcol; x++; }
		} else {
			l = -1;
		}
	}

	// account for possible leak to the right
	if ((l != -1) && (x > x2)) { 
		scanSpan( -2, -dir, x2+1, x-1, y-dir, m );
		scanSpan(  2,  dir,    l, x-1, y+dir, m );
	}
}


long long int IntensityImage::integrate( Point start, Vector& centroid, Vector& axis1, Vector& axis2, unsigned char oldcol, unsigned char newcol, std::vector<Point>* border ) {

	Moments m = { 0, 0, 0, 0, 0, 0, oldcol, newcol };

	//m.m00 = integrate( start.x, start.y, &m );

	scanSpan( 0, -1, start.x, start.x, start.y, &m );
	scanSpan( 0,  1, start.x, start.x, start.y, &m );

	//std::cout << "moments: m00 = " << m00 << " m10 = " << m10 << " m01 = " << m01 << " m11 = " << m11 << " m20 = " << m20 << " m02 = " << m02 << std::endl;
	
	// calculate centroid: 1st order central moments
	double cx = (double)m.m10/(double)m.m00; centroid.x = cx;
	double cy = (double)m.m01/(double)m.m00; centroid.y = cy;

	// 2nd order central moments
	double mu20 = (double)m.m20/(double)m.m00 - cx*cx;
	double mu11 = (double)m.m11/(double)m.m00 - cx*cy;
	double mu02 = (double)m.m02/(double)m.m00 - cy*cy;

	// calculate eigenvalues lambda1,2 of image covariance matrix [ (mu20 mu11) (mu11 mu02) ]
	double tmp1 = mu20 + mu02;
	double tmp2 = mu20 - mu02;
	double tmp3 = sqrt( 4.0*mu11*mu11 + tmp2*tmp2 );

	double lambda1 = (tmp1 + tmp3)/2.0;
	double lambda2 = (tmp1 - tmp3)/2.0;

	// calculate eigenvectors using first matrix equation y = x * (lambda1,2 - mu20) / mu11
	axis1 = Vector( 1.0, ((lambda1-mu20)/mu11) );
	axis2 = Vector( 1.0, ((lambda2-mu20)/mu11) );

	// if result = NaN: repeat calculation using second equation y = x * mu11 / (lambda1,2 - mu02)
	if (isnan(axis1.y)) axis1.y = (mu11/(lambda1-mu02)); 
	if (isnan(axis2.y)) axis2.y = (mu11/(lambda2-mu02)); 

	// if result = Inf: normalize to base vector
	if (isinf(axis1.y)) axis1.set( 0.0, 1.0 );
	if (isinf(axis2.y)) axis2.set( 0.0, 1.0 );

	// normalize to axis lengths
	axis1.normalize(); axis1 = axis1 * sqrt( 3.0*fabs(lambda1) );
	axis2.normalize(); axis2 = axis2 * sqrt( 3.0*fabs(lambda2) );

	/* alternative angle calculation:
	double t = (mu02-mu20)/(2.0*mu11);
	double angle2 = atan(t+sqrt(t*t+1));*/
	centroid.z = 0.5 * atan( 2.0*mu11 / ( mu20 - mu02 ) );

	if (!border) return m.m00;

	// scan the border
	int neighbor[] = { -width-1, -width, -width+1, +1, width+1, width, width-1, -1 };

	int offs0 = start.y * width + start.x;
	int offset = offs0;
	int found = 0;

	while ((offset != offs0) || (!found)) {
		found = 0;
		for (int i = 0; i < 8; i++)
			if ((data[offset+neighbor[i]] == newcol) && (data[offset+neighbor[(i+1)%8]] != newcol)) {
				border->push_back( Point(offset%width,offset/width) );
				offset += neighbor[i];
				found = 1;
				break;
			}
		if (!found) { offset++; offs0 = offset; }
	}

	return m.m00;
}


void IntensityImage::despeckle( IntensityImage& target, unsigned char threshold ) const {
	#ifndef NOMMX
		memset( target.data, 0, width );
		memset( target.data+(width*(height-1)), 0, width );
		for (int j = 0; j < width-7; j+=6) mmxdespeckle( data+j, target.data+j, height, width, threshold );
		mmxdespeckle( data+width-7, target.data+width-7, height, width, threshold );
	#else
		int neighbor[] = { -width-1, -width, -width+1, -1, 0, +1, width-1, width, width+1 };
		int sum;
		target.clear();
		for (int i = width+1; i < count-width-1; i++) {
			sum = 0;
			for (int j = 0; j < 9; j++) if (data[i+neighbor[j]]) sum++;
			target.data[i] = (sum >= threshold) ? 255 : 0;
		}
	#endif
}


void IntensityImage::houghLine( IntensityImage& target ) const {

	double theta,rho;
	double pi_n = M_PI / (double)target.width;
	unsigned char tmp;
	int offset, max = 0;

	int  size = target.width * target.height;
	int* accu = new int[ size ];
	for (int i = 0; i < size; i++) accu[i] = 0;

	target.clear();
	
	for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) {
	
		tmp = getPixel(x,y); 	
		if (tmp == 0) continue;
		
		for (int j = 0; j < target.width; j++) {
			
			theta = pi_n * (double)j;
			rho = (double)x/(double)width * cos(theta) + (double)y/(double)height * sin(theta);
			
			rho = (rho+1.0)/2.5;
			rho = rho * (double)target.height;

			offset = target.pixelOffset(j,(int)round(rho));
			/*tmp = target.data[ offset ];
			target.data[ offset ] = (tmp=255)?255:tmp+1;*/
			accu [ offset ] += tmp ;
		}
	}

	for (int i = 0; i < size; i++) if (accu[i] > max) max = accu[i];
	for (int i = 0; i < size; i++) target.data[i] = (unsigned char) round( ((double)accu[i] * 255.0) / (double)max );

	//std::cout << max << std::endl;

	delete[] accu;
}


void IntensityImage::gradient( char* xgrad, char* ygrad ) {

	int xval,yval,offs;

	for (int x = 1; x < width-1; x++) for (int y = 1; y < height-1; y++) {
		
		offs = pixelOffset(x,y);
		
		xval = getPixel(x+1,y) - getPixel(x-1,y);
		yval = getPixel(x,y-1) - getPixel(x,y+1);
		
		xgrad[offs] = xval/2;
		ygrad[offs] = yval/2;
	}
}


void IntensityImage::sobel( unsigned char* target ) {
	
	int xval,yval,value;

	for (int x = 1; x < width-1; x++) for (int y = 1; y < height-1; y++) {

		xval =   getPixel(x+1,y-1) -   getPixel(x-1,y-1)
				 + 2*getPixel(x+1,y  ) - 2*getPixel(x-1,y  )
				 +   getPixel(x+1,y+1) -   getPixel(x-1,y+1);

		yval =   getPixel(x-1,y-1) -   getPixel(x-1,y+1)
				 + 2*getPixel(x  ,y-1) - 2*getPixel(x  ,y+1)
				 +   getPixel(x+1,y-1) -   getPixel(x+1,y+1);

		value = (int)sqrt((double)(xval*xval+yval*yval));
		target[pixelOffset(x,y)] = value/4;
	}
}

void IntensityImage::sobel( IntensityImage& target ) {
	sobel( target.data );
}

void IntensityImage::sobel() {
	// evil pointer shuffling
	unsigned char* tmp = new unsigned char[size];
	sobel( tmp );
	delete[] data;
	data = tmp;
}


void IntensityImage::adaptive_threshold( int radius, int bias, IntensityImage& target ) const {

	int* colsum = new int[width];
	int colcnt = radius+1;

	// initialize column sums
	for (int x = 0; x < width; x++) {
		colsum[x] = 0;
		for (int y = 0; y <= radius; y++)
			colsum[x] += getPixel(x,y);
	}

	for (int y = 0; y < height; y++) {

		for (int x = 0; x < width; x++) {

			// add the column sums from -radius to +radius
			int thr = 0, cnt = 0;
			for (int i = x-radius; i <= x+radius; i++) {
				if ((i < 0) || (i >= width)) continue;
				thr += colsum[i];
				cnt += colcnt;
			}

			// calculate threshold from mean and bias
			thr /= cnt;
			thr -= bias;

			// apply to pixel
			target.setPixel( x, y, ( (getPixel(x,y) > thr) ? 255 : 0 ) );
		}

		// update column sums
		int lower = y-radius-1;
		int upper = y+radius+1;

		if (lower >=    0) { colcnt--; for (int x = 0; x < width; x++) colsum[x] -= getPixel( x, lower ); }
		if (upper < width) { colcnt++; for (int x = 0; x < width; x++) colsum[x] += getPixel( x, upper ); }
	}
	delete colsum;
}

int IntensityImage::threshold( unsigned char value ) {
	return threshold( value, *this );
}

int IntensityImage::threshold( unsigned char value, IntensityImage& target , unsigned char minvalue ) const {
	#ifndef NOMMX
		mmxthreshold( data, target.data, size, value );
		return 0;
	#else
		unsigned char tmp;
		int hits = 0;
		for (int i = 0; i < count; i++) {
			tmp = ((data[i] > value) && (data[i] <= minvalue)) ? 255 : 0;
			if (tmp) hits++;
			target.data[i] = tmp;
		}
		return hits;
	#endif
}


void IntensityImage::undistort( Vector scale, Vector delta, double coeff[5], IntensityImage& target ) const {

	Vector temp;
	target.clear();

	for (int u = 0; u < width; u++) for (int v = 0; v < height; v++) {

		temp = Vector( u, v, 0 );
		::undistort( temp, scale, delta, coeff );

		if ((temp.x < 0) || (temp.x >=  width)) continue;
		if ((temp.y < 0) || (temp.y >= height)) continue;
		target.setPixel( u, v, getPixel( (int)temp.x, (int)temp.y ) );
	}
}


void IntensityImage::invert( ) {
	invert( *this );
}

void IntensityImage::invert( IntensityImage& target ) const {
	unsigned char tmp;
	for (int i = 0; i < count; i++) {
		tmp = 255-data[i];
		target.data[i] = tmp;
  }
}


IntensityImage& IntensityImage::operator-=( const IntensityImage& arg ) {
	int tmp;
	for (int i = 0; i < count; i++) {
		tmp = (int)data[i] - (int)arg.data[i];
		data[i] = (tmp > 0) ? ((tmp < 255) ? tmp : 255 ) : 0;
	}
	return *this;
}


void IntensityImage::subtract( const IntensityImage& i1, const IntensityImage& i2 ) {
	int tmp;
	for (int i = 0; i < count; i++) {
		tmp = (int)i1.data[i] - (int)i2.data[i];
		data[i] = (tmp > 0) ? ((tmp < 255) ? tmp : 255 ) : 0;
	}
}


std::ostream& operator<<( std::ostream& s, const IntensityImage& i ) {
	s << "P5 " << i.width << " " << i.height << " 255 ";
	s.write( (char*)i.data, i.size );
	return s;	
}

