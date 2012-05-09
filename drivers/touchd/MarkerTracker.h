/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MARKERTRACKER_H_
#define _MARKERTRACKER_H_

#define MY_DEBUG 1;

#include "PoseEstimation.h"

#include "IntensityImage.h"
#include "ShortImage.h"
#include "RGBImage.h"

#include "opencv2/imgproc/imgproc.hpp"

#ifdef MY_DEBUG
#include "opencv2/highgui/highgui.hpp"
#endif

using namespace std;

class MarkerTracker {
public:
	MarkerTracker(unsigned char thresh, unsigned char bw_thresh, unsigned char max, int imgwidth, int imgheight);
	virtual ~MarkerTracker();

	void update_thresh( unsigned char new_thresh );
	void update_bw_thresh( unsigned char new_bw_thresh );

	int subpixSampleSafe( cv::Mat pSrc, CvPoint2D32f p );
	void findMarker( RGBImage* rgbimage, IntensityImage* image );
	//void findSquares( cv::Mat& matThresholded, cv::Mat& matRGBImage, cv::Mat& matConverted );
	//void drawSquares( cv::Mat& matRGBImage );
	//void detectBorder( cv::Mat& matRGBImage, cv::Mat& matConverted, vector<cv::Vec4f>* lineParams );
	//void drawDetectedBorder( cv::Mat& matRGBImage, vector<cv::Vec4f>* lineParams );
	//void detectExactCorners( cv::Mat& matRGBImage, cv::Mat& matConverted, vector<cv::Vec4f>* lineParams );
	//void identifyMarker( );


protected:
	
	//CvMemStorage* memStorage;
	bool isFirstStripe;
	bool isFirstMarker;
	CvSize picSize;
	//vector<vector<cv::Point> > squares;
	//cv::Point2f corners[4];
	//int amountOfContours;
	cv::Mat matMarker;

	cv::Scalar cv_red;
	cv::Scalar cv_green;
	cv::Scalar cv_blue;
	cv::Scalar cv_yellow;
	cv::Scalar cv_magenta;
	cv::Scalar cv_cyan;

	unsigned char thresh;
	unsigned char bw_thresh;
	unsigned char max;

};

#endif _MARKERTRACKER_H_