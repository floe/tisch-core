/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _CAMERA_H_
#define _CAMERA_H_


#include "FlashControl.h"
#include "ImageSource.h"
#include "Filter.h"


#define CAMERA_TYPE_V4L        1
#define CAMERA_TYPE_DC1394     2
#define CAMERA_TYPE_DIRECTSHOW 3
#define CAMERA_TYPE_BIGTOUCH   4
#define CAMERA_TYPE_KINECT     5
#define CAMERA_TYPE_FFMV       6


class Camera: public Filter {

	public:

		Camera( TiXmlElement* _config, Filter* _input );
		~Camera();

		virtual int process( );

		void tilt_kinect( int angle );

		// Configurator
		virtual TiXmlElement* getXMLRepresentation();

	private:

		FlashControl* flash;
		ImageSource*  cam;

		int verbose;
		int width, height, fps, sourcetype;
		int gain, expo, bright, flashmode;
		std::string sourcepath, flashpath;
};

#endif // _CAMERA_H_

