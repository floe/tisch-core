/*
 * Ubitrack - Library for Ubiquitous Tracking
 * Copyright 2006, Technische Universitaet Muenchen, and individual
 * contributors as indicated by the @authors tag. See the
 * copyright.txt in the distribution for a full listing of individual
 * contributors.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */

/**
 * @ingroup api
 * @file
 * Implements a simple interface to the ubitrack dataflow network.
 *
 * @author Daniel Pustka <daniel.pustka@in.tum.de>
 */

#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <boost/bind.hpp>
//#include <log4cpp/Category.hh>

#include "SimpleMarkerDetection.h"
#include "../MarkerDetection.h"


namespace Ubitrack { namespace Vision {

/*
 * Detects markers in the image.
 *
 * @param img Greyscale image
 * @param simpleMarkers vector of SimpleMarkerInfo where found Markers are saved 
 */
void findMarkers(IplImage* img, std::vector<SimpleMarkerInfo>& simpleMarkers, std::vector<int>& markerList, IplImage* debugimg ) throw()
{
	// compute cheap camera matrix if none given
	Math::Matrix< 3, 3, float > K;
	float fTx = static_cast< float >( img->width / 2 );
	float fTy = static_cast< float >( img->height / 2 );
	float f = 1.25f * img->width;
	K( 0, 0 ) = f;
	K( 0, 1 ) = 0.0f;
	K( 0, 2 ) = -fTx;
	K( 1, 1 ) = f;
	K( 1, 2 ) = -fTy;
	K( 2, 2 ) = -1.0f;
	K( 1, 0 ) = K( 2, 0 ) = K( 2, 1 ) = 0.0f;

	Image myimg(img, false);

	Markers::MarkerInfo temp(6);
	Markers::MarkerInfoMap markermap;
	for(std::vector<int>::iterator it = markerList.begin(); it!=markerList.end(); it++)
	{
		Math::Scalar< unsigned long long int > ID = *it;
		markermap[ ID ] = temp;
	}

	if(!debugimg)
		Markers::detectMarkers( myimg, markermap, K);
	else
	{
		Image* mydebugimg = new Image(debugimg->width, debugimg->height, debugimg->nChannels, debugimg->imageData);
		Markers::detectMarkers( myimg, markermap, K, mydebugimg);
	}


	//markers in simplemarkers übertragen
	for( std::map<unsigned long long int, Markers::MarkerInfo>::iterator it = markermap.begin(); it!=markermap.end(); it++)
	{
		if(!(*it).second.found)
			continue;
		SimpleMarkerInfo MySimpleMarkerInfo;
		MySimpleMarkerInfo.ID = (*it).first;

		//Copy Corners
		MySimpleMarkerInfo.corners[0].x = (*it).second.corners[0][0];
		MySimpleMarkerInfo.corners[0].y = (*it).second.corners[0][1];
		MySimpleMarkerInfo.corners[1].x = (*it).second.corners[1][0];
		MySimpleMarkerInfo.corners[1].y = (*it).second.corners[1][1];
		MySimpleMarkerInfo.corners[2].x = (*it).second.corners[2][0];
		MySimpleMarkerInfo.corners[2].y = (*it).second.corners[2][1];
		MySimpleMarkerInfo.corners[3].x = (*it).second.corners[3][0];
		MySimpleMarkerInfo.corners[3].y = (*it).second.corners[3][1];

		//Copy Pose - Translation
		Math::Vector< 3 >	m_translation = (*it).second.pose.translation();
		MySimpleMarkerInfo.pos.reserve(3);
		MySimpleMarkerInfo.pos.push_back( m_translation[0] ); 
		MySimpleMarkerInfo.pos.push_back( m_translation[1] ); 
		MySimpleMarkerInfo.pos.push_back( m_translation[2] );
		
		//Copy Pose - Rotation
		Math::Quaternion m_rotation = (*it).second.pose.rotation();
		MySimpleMarkerInfo.rot.reserve(4);
		MySimpleMarkerInfo.rot.push_back( m_rotation.x() ); 
		MySimpleMarkerInfo.rot.push_back( m_rotation.y() ); 
		MySimpleMarkerInfo.rot.push_back( m_rotation.z() ); 
		MySimpleMarkerInfo.rot.push_back( m_rotation.w() ); 

		simpleMarkers.push_back(MySimpleMarkerInfo);
	}
}

} }
