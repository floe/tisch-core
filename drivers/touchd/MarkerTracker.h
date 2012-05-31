/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MARKERTRACKER_H_
#define _MARKERTRACKER_H_

#define MT_DEBUG

#include "PoseEstimation.h"

#include "IntensityImage.h"
#include "ShortImage.h"
#include "RGBImage.h"

#include "SimpleMarkerDetection.h"
#include "opencv2/imgproc/imgproc.hpp"

#ifdef MT_DEBUG
#include "opencv2/highgui/highgui.hpp"
#endif

using namespace std;

class MarkerTracker {
public:
	MarkerTracker(int imgwidth, int imgheight);
	virtual ~MarkerTracker();

	void addMarkerID(int markerID);
	int subpixSampleSafe( cv::Mat pSrc, CvPoint2D32f p );
	void findMarker( RGBImage* rgbimage, IntensityImage* image, std::vector<Ubitrack::Vision::SimpleMarkerInfo>* foundMarkers );
	
	
protected:
	
	CvSize picSize;
	
	cv::Scalar cv_red;
	cv::Scalar cv_green;
	cv::Scalar cv_blue;
	cv::Scalar cv_yellow;
	cv::Scalar cv_magenta;
	cv::Scalar cv_cyan;

	unsigned char thresh;
	unsigned char bw_thresh;
	unsigned char max;

	IplImage* iplRGBImage;
	IplImage* iplConverted;
	std::vector<Ubitrack::Vision::SimpleMarkerInfo>* detectedMarkers;
    std::vector<int>* markerList;

};

#endif _MARKERTRACKER_H_