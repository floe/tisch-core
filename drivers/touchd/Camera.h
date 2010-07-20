/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _CAMERA_H_
#define _CAMERA_H_


#include "FlashControl.h"
#include "ImageSource.h"
#include "Settings.h"


class Camera {

	public:

		Camera(
			VideoSettings* vidset, int verbose,
			const char* videodev = "/dev/video0",
			const char* ctrlport = "/dev/ttyS0"
		);

		~Camera();

		// apply settings 
		void apply( CameraSettings* camset );

		// get picture from the camera and store into image
		void acquire( IntensityImage* target );

	private:

		FlashControl* flash;
		ImageSource* cam;
};

#endif // _CAMERA_H_

