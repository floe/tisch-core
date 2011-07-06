/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "FFMVImageSource.h"
#include "DCImageSource.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stdexcept>

#define LOG4CPP_INFO(x,y) x << y << std::endl;
#define LOG4CPP_WARN(x,y) x << y << std::endl;
#define LOG4CPP_DEBUG(x,y) x << y << std::endl;

FFMVImageSource::FFMVImageSource( int dwidth, int dheight, const char* videodev, int verbose ) {
		
		camera = new FlyCapture2::Camera();

		std::stringstream stream;
		stream << videodev;
		int cameraIndex;
		stream >> cameraIndex;
		//printf("camera index %i", cameraIndex);

		//getting the camera identifier
		// TODO -> get camera from source string
		FlyCapture2::PGRGuid guid;
		error = busMgr.GetCameraFromIndex( cameraIndex, &guid );
		if( error != FlyCapture2::PGRERROR_OK )
			printError( error );
		
		// Connect to camera
		// TODO -> get camera from source string
		error = camera->Connect( &guid );
		if( error != FlyCapture2::PGRERROR_OK )
			printError( error );
		
		// Get the camera information and printing it
		FlyCapture2::CameraInfo camInfo;
		error = camera->GetCameraInfo( &camInfo );
		if( error != FlyCapture2::PGRERROR_OK )
			printError( error );
		//printCameraInfo( &camInfo ); 

		error = camera->SetVideoModeAndFrameRate(
			FlyCapture2::VIDEOMODE_640x480Y8, 
			FlyCapture2::FRAMERATE_30);

		if( error != FlyCapture2::PGRERROR_OK )
			printError( error );

		image = new FlyCapture2::Image();

		microsecondsLastFrame = 0;
		secondsLastFrame = 0;
		frameCounter = 0;

		//TODO: starting camera capture -> done by start()?
		/*
		error = camera->StartCapture();
		if( error != PGRERROR_OK )
			printError( error );
		*/
		
		imgbuffer = new RGBImage(640,480);
		intensityImgbuffer = new IntensityImage(640, 480);
		running = 0;

		// switch GPIO0-3 to output
		unsigned int reg; error = camera->ReadRegister( PIO_DIRECTION, &reg );
		camera->WriteRegister( PIO_DIRECTION, reg | 0xF0000000 );

		// enable GPIO0-3 10us high strobe on exposure start
		camera->WriteRegister( STROBE_0_CNT, 0x83000020 );
		camera->WriteRegister( STROBE_1_CNT, 0x83000020 );
		camera->WriteRegister( STROBE_2_CNT, 0x83000020 );
		camera->WriteRegister( STROBE_3_CNT, 0x83000020 );
}

FFMVImageSource::~FFMVImageSource(){
	camera->StopCapture();
	camera->Disconnect();
	delete camera;
	delete image;
}

void FFMVImageSource::start() {
	if ( !running ) {
		error = camera->StartCapture();
		if( error != FlyCapture2::PGRERROR_OK ) {
			printError( error );
		}
	}
	running = 1;
}


void FFMVImageSource::stop() {
	if ( running )  {
		error = camera->StopCapture();
		if( error != FlyCapture2::PGRERROR_OK ) {
			printError( error );
		}
	}
	running = 0;
}

// get image data from camera
int  FFMVImageSource::acquire( ) { 
	// Grab the current frame for the index camera
	error = camera->RetrieveBuffer(image);
	if(error != FlyCapture2::PGRERROR_OK) {
		printError(error);
	}

	// TODO: Do any new frame checks?
	/*
	TimeStamp timestamp = image->GetTimeStamp();

	if(frameCounter != 0) {
		if(timestamp.cycleSeconds < secondsLastFrame[cameraIndex]) {
			//isNewFrame = false;
			continue;
		}
		else if(timestamp.cycleSeconds == secondsLastFrame[cameraIndex]) {
			if(microsecondsLastFrame[cameraIndex] >= timestamp.microSeconds) {
				//isNewFrame = false;
				continue;
			}
		}
	}

	//isNewFrame = true;
	microsecondsLastFrame = timestamp.microSeconds ;
	secondsLastFrame = timestamp.cycleSeconds ;

	//printf( "Cam %d - Frame %d - TimeStamp [%d %d]\n", cameraIndex , frameCounter++ , timestamp.cycleSeconds, timestamp.cycleCount);
	*/

	//Copy image data to image buffer
	// TODO: correct destination?
	memcpy(imgbuffer->getData(), image->GetData(), image->GetDataSize());
	memcpy(intensityImgbuffer->getData(), image->GetData(), image->GetDataSize());

	frameCounter++;

	// TODO: What to return?
	return 1;
}

void FFMVImageSource::release( ) {
	// TODO
}

// application gets image data from here
void FFMVImageSource::getImage( IntensityImage& target ) const {
	//if (imgbuffer) imgbuffer->getIntensity( target );
	if(intensityImgbuffer) target = *intensityImgbuffer;
}

// application gets image data from here
void FFMVImageSource::getImage( RGBImage& target ) const {
	if (imgbuffer) target = *imgbuffer;
}

void FFMVImageSource::printError( FlyCapture2::Error error ){
	printf( "%s\n" , error.GetDescription() );
}

void FFMVImageSource::printCameraInfo( FlyCapture2::CameraInfo* pCamInfo ){
	printf(
		"\n*** CAMERA INFORMATION ***\n"
		"Serial number - %u\n"
		"Camera model - %s\n"
		"Camera vendor - %s\n"
		"Sensor - %s\n"
		"Resolution - %s\n"
		"Firmware version - %s\n"
		"Firmware build time - %s\n\n",
		pCamInfo->serialNumber,
		pCamInfo->modelName,
		pCamInfo->vendorName,
		pCamInfo->sensorInfo,
		pCamInfo->sensorResolution,
		pCamInfo->firmwareVersion,
		pCamInfo->firmwareBuildTime );
}

void FFMVImageSource::set_feature( FlyCapture2::PropertyType feature, int value ) {

	if (feature >= FlyCapture2::UNSPECIFIED_PROPERTY_TYPE) return;

	Property prop( feature );
	usleep( SAFETY_DELAY );

	if (value == IMGSRC_OFF) {

		prop.autoManualMode = false; 
		prop.onOff = false;
		camera->SetProperty( &prop );

	} else if (value == IMGSRC_AUTO) {

		prop.autoManualMode = true; 
		prop.onOff = true;
		camera->SetProperty( &prop );

	} else {

		prop.autoManualMode = false; 
		prop.absControl = true;
		prop.absValue = value;
		camera->SetProperty( &prop );

	}
}

// dummy functions for now
void FFMVImageSource::setGain      ( int gain   ) { set_feature( FlyCapture2::GAIN,          gain   ); }
void FFMVImageSource::setShutter   ( int speed  ) { set_feature( FlyCapture2::SHUTTER,       speed  ); }
void FFMVImageSource::setExposure  ( int exp    ) { set_feature( FlyCapture2::AUTO_EXPOSURE, exp    ); }
void FFMVImageSource::setBrightness( int bright ) { set_feature( FlyCapture2::BRIGHTNESS,    bright ); }

void FFMVImageSource::setFPS( int fps ) {}
void FFMVImageSource::printInfo( int feature ) {}
