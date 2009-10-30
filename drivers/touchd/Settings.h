/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Vector.h"


#define CAMERA_TYPE_V4L        1
#define CAMERA_TYPE_DC1394     2
#define CAMERA_TYPE_DIRECTSHOW 3


class Camera;

struct VideoSettings {
	int width, height, fps, source;
	int startgain, startexpo, startbright, startflash;
};

struct CameraSettings {
	int apply, gain, exposure, flash;
};

struct VisionSettings {
	int threshold, invert, noise;
	double bgfactor;
};

struct BlobSettings {
	int minsize, maxsize, gid;
	double factor, radius, peakdist;
	Color cross;
	Color trail;
};


struct Settings {

	CameraSettings camera;
	VisionSettings vision;
	BlobSettings   blob;

	VideoSettings* video;

	const char* name;
};

std::ostream& operator<<( std::ostream& s, Settings& set );
std::istream& operator>>( std::istream& s, Settings& set );


struct PipelineParameters {

	Camera*       camera;
	std::ostream* output;

	Settings settings;

};

#endif // _SETTINGS_H_

