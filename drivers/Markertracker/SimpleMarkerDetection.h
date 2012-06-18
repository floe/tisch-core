
#ifndef __UBITRACK_VISION_SIMPLEMARKERDETECTION_H_INCLUDED__
#define __UBITRACK_VISION_SIMPLEMARKERDETECTION_H_INCLUDED__

#define UBITRACK_EXPORT __declspec( dllimport )

namespace Ubitrack { namespace Vision {

struct SimpleMarkerInfo
{
	int ID; //Marker ID
	cv::Point2f corners[4]; //Corners
	std::vector<double> pos; //Pose Position x,y,z
	std::vector<double> rot; //Pose Rotation
};

/**
 * Detect Markers in an IplImage and save them in a vector of SimpleMarkerInfo.
 */
UBITRACK_EXPORT void findMarkers(IplImage* img, std::vector<SimpleMarkerInfo>& simpleMarkers, std::vector<int>& markerList, IplImage* debugimg) throw();

 } } 

#endif
