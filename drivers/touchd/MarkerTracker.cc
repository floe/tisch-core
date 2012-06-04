/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
 *     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
 *   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
 \*************************************************************************/

#include "MarkerTracker.h"

/*
* Constructor
*/
MarkerTracker::MarkerTracker(int _imgwidth, int _imgheight) {
	picSize.width = _imgwidth;
	picSize.height = _imgheight;

	iplRGBImage = cvCreateImage( picSize, IPL_DEPTH_8U, 3 );
	iplConverted = cvCreateImage( picSize, IPL_DEPTH_8U, 1 );
	
	markerList = new std::vector<int>;

	cv_red		= CV_RGB(255,0,0);
	cv_green	= CV_RGB(0,255,0);
	cv_blue		= CV_RGB(0,0,255);
	cv_yellow	= CV_RGB(0,255,255);
	cv_magenta	= CV_RGB(255,0,255);
	cv_cyan		= CV_RGB(255,255,0);


#ifdef MT_DEBUG
	cv::namedWindow("Debug", CV_WINDOW_AUTOSIZE);
#endif
}

MarkerTracker::~MarkerTracker() {
	cvReleaseImage(&iplRGBImage);
	cvReleaseImage(&iplConverted);
	delete markerList;
#ifdef MT_DEBUG
	cvDestroyAllWindows();
#endif
}

/*
*
*/
int MarkerTracker::subpixSampleSafe ( cv::Mat pSrc, CvPoint2D32f p )
{
	int x = int( floorf ( p.x ) );
	int y = int( floorf ( p.y ) );

	if ( x < 0 || x >= pSrc.cols - 1 ||
		 y < 0 || y >= pSrc.rows - 1 )
		return 127;

	int dx = int ( 256 * ( p.x - floorf ( p.x ) ) );
	int dy = int ( 256 * ( p.y - floorf ( p.y ) ) );

	unsigned char* i = ( unsigned char* ) ( ( pSrc.data + y * pSrc.step ) + x );
	int a = i[ 0 ] + ( ( dx * ( i[ 1 ] - i[ 0 ] ) ) >> 8 );
	i += pSrc.step;
	int b = i[ 0 ] + ( ( dx * ( i[ 1 ] - i[ 0 ] ) ) >> 8 );

	return a + ( ( dy * ( b - a) ) >> 8 );
}

/*
* 
*/
void MarkerTracker::addMarkerID(int markerID) {
	markerList->push_back(markerID);
}

/*
* main function called by process function in MarkerTrackerFilter from Filter.cc
*/
void MarkerTracker::findMarker(RGBImage* rgbimage, IntensityImage* image, std::vector<Ubitrack::Vision::SimpleMarkerInfo>* detectedMarkers) {
	unsigned char* org_rgb_image_data = rgbimage->getData(); // pointer to unsigned char array
	//unsigned char* org_intensity_image_data = image->getData();

	cvSetData( iplRGBImage, org_rgb_image_data, picSize.width * 3 ); 

	// convert RGB to gray scale image
	cvCvtColor( iplRGBImage, iplConverted, CV_RGB2GRAY ); // libTISCH RGB images bytes are ordered in RGB not BGR like in openCV!
	
	IplImage* iplDebug = cvCreateImage( picSize, IPL_DEPTH_8U, 3);
	cvCvtColor(iplConverted, iplDebug, CV_GRAY2RGB);
	
	detectedMarkers->clear();

    Ubitrack::Vision::findMarkers(iplConverted, *detectedMarkers, *markerList, iplDebug);

#ifdef MT_DEBUG
	if(!detectedMarkers->empty())
		std::cout << std::hex << setfill('0') << setw(2) << nouppercase << detectedMarkers->at(0).ID << std::endl;

	cvShowImage("Debug", iplDebug);
    
#endif
	cvReleaseImage(&iplDebug);
}
