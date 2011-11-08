/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <algorithm>
#include <stdexcept>
#include <sstream>

#include "DCImageSource.h"

// helper function to sort the camera array 
bool cmp( dc1394camera_id_t a, dc1394camera_id_t b )
{
	return a.guid > b.guid;
}


DCImageSource::DCImageSource( int _width, int _height, int _fps, int _num, int _verbose ) {

	width  = _width;
	height = _height;
	fps = _fps;

	verbose = _verbose;
	num = _num;

	framecount = 0;
	camera = 0;
	frame = 0;

	context = dc1394_new();

	// find cameras
	if (dc1394_camera_enumerate( context, &list ) != DC1394_SUCCESS) {
		dc1394_free( context );
		throw std::runtime_error( "Unable to scan for IIDC cameras. Check your Firewire bus." );
	}

	if (list->num <= (unsigned int)num) {
		dc1394_free( context );
		throw std::runtime_error( "No IIDC camera with requested index found." );
	}

	// Sort the cameras according to their GUID for a consistent ordering.
	std::sort( list->ids, list->ids + list->num, cmp );
	camera = new dc1394camera_t*[list->num];

	for (unsigned int i = 0; i < list->num; i++) camera[i] = dc1394_camera_new( context, list->ids->guid );

	// setup capture parameters
  camera_setup( );
}


DCImageSource::~DCImageSource() {
	if (verbose) std::cout << "Doing cleanup for camera #" << num << "." << std::endl;
	stop( );
	dc1394_capture_stop( camera[num] );
	for (unsigned int i = 0; i < list->num; i++) dc1394_camera_free( camera[i] );
	delete[] camera;
	dc1394_camera_free_list( list );
	dc1394_free( context );
}


void DCImageSource::start( ) { dc1394_video_set_transmission( camera[num], DC1394_ON  ); }
void DCImageSource::stop( )  { dc1394_video_set_transmission( camera[num], DC1394_OFF ); }


void DCImageSource::camera_setup( ) {

 	if (verbose) std::cout << "Setting up camera #" << num << ".." << std::endl;

	dc1394_reset_bus( camera[num] );
	dc1394_camera_reset( camera[num] );
	dc1394_iso_release_all( camera[num] );
	dc1394_video_set_transmission( camera[num], DC1394_OFF );

	dc1394_video_set_iso_speed( camera[num], DC1394_ISO_SPEED_400 );

	if (fps == 0) { // format7 free running mode

		dc1394_video_set_mode( camera[num], DC1394_VIDEO_MODE_FORMAT7_0 );

		dc1394error_t result = dc1394_format7_set_roi(
			camera[num], DC1394_VIDEO_MODE_FORMAT7_0,
			DC1394_COLOR_CODING_MONO8, DC1394_USE_MAX_AVAIL,
			0, 0, width, height );

		if (result != DC1394_SUCCESS)
			throw std::runtime_error( "Error during camera initialization. Check that the requested Format7 mode is supported." );

	} else {

		if ((width ==  640) && (height == 480)) dc1394_video_set_mode( camera[num], DC1394_VIDEO_MODE_640x480_MONO8  ); else
		if ((width ==  800) && (height == 600)) dc1394_video_set_mode( camera[num], DC1394_VIDEO_MODE_800x600_MONO8  ); else
		if ((width == 1024) && (height == 768)) dc1394_video_set_mode( camera[num], DC1394_VIDEO_MODE_1024x768_MONO8 ); else
		throw std::runtime_error( "Unsupported framesize. Choose 640x480, 800x600 or 1024x768." );

		switch (fps) {
			case  15: dc1394_video_set_framerate( camera[num], DC1394_FRAMERATE_15  ); break;
			case  30: dc1394_video_set_framerate( camera[num], DC1394_FRAMERATE_30  ); break;
			case  60: dc1394_video_set_framerate( camera[num], DC1394_FRAMERATE_60  ); break;
			case 120: dc1394_video_set_framerate( camera[num], DC1394_FRAMERATE_120 ); break;
			default: throw std::runtime_error( "Unsupported framerate. Choose 15, 30, 60 or 120 Hz." );
		}
	}

	if (dc1394_capture_setup( camera[num], NUM_DMA_BUFFERS, DC1394_CAPTURE_FLAGS_DEFAULT ) != DC1394_SUCCESS)
		throw std::runtime_error( "Error during capture setup." );
 
	dc1394_feature_get_all( camera[num], &features );

	if (verbose) {
		std::cout << "Camera #" << num << " setup successful." << std::endl;
		dc1394_feature_print_all( &features, stdout );
		uint32_t unit,max,ps;
		dc1394_format7_get_packet_parameters( camera[num], DC1394_VIDEO_MODE_FORMAT7_0, &unit, &max );
		std::cout << "Maximum packet size: " << max << " bytes in units of " << unit << " bytes." << std::endl;
		dc1394_format7_get_packet_size( camera[num], DC1394_VIDEO_MODE_FORMAT7_0, &ps );
		std::cout << "Current packet size: " << ps << " bytes." << std::endl;
	}

	// enable frame counter embedding
	int reg = getReg( FRAME_INFO );
	if (reg & 0x80000000) setReg( FRAME_INFO, reg | 1 );

	start();
}


/** convert YUV411 image data to greyscale - this function is missing in libdc1394 */
void dc1394_YUV411_to_MONO8( uint8_t* src, uint8_t* dest, uint32_t width, uint32_t height ) {

  register int i = (width*height) + ( (width*height) >> 1 ) -1;
  register int j = (width*height)                           -1;
  register int y0, y1, y2, y3;
  
  while (i >= 0) {
    y3 = (uint8_t) src[i--];
    y2 = (uint8_t) src[i--]; i--;
    y1 = (uint8_t) src[i--];
    y0 = (uint8_t) src[i--]; i--;
    dest[j--] = y3;
    dest[j--] = y2;
    dest[j--] = y1;
    dest[j--] = y0;
  }
}


int DCImageSource::acquire( ) {

	// retrieve raw image data
	dc1394_capture_dequeue( camera[num], DC1394_CAPTURE_POLICY_WAIT, &frame ); if (!frame) return 0;
	rawdata = (unsigned char*)(frame->image);

	if (frame->frames_behind > 0)
		if (verbose) std::cout << "current queue length: " << frame->frames_behind << " frame(s))" << std::endl;

	unsigned int info0 = rawdata[0];
	unsigned int info1 = rawdata[1];
	unsigned int info2 = rawdata[2];
	unsigned int info3 = rawdata[3];

	unsigned int seconds = (info0) >> 1;
	unsigned int cycles = ((info0&0x01) << 12) | (info1 << 4) | (info2 >> 4);
	unsigned int clocks = ((info2&0x0F) <<  8) | info3;

	/*
	inline double 
	imageTimeStampToSeconds(unsigned int uiRawTimestamp)
	{

		 int nSecond      = (uiRawTimestamp >> 25) & 0x7F;   // get rid of cycle_* - keep 7 bits
		 int nCycleCount  = (uiRawTimestamp >> 12) & 0x1FFF; // get rid of offset
		 int nCycleOffset = (uiRawTimestamp >>  0) & 0xFFF;  // get rid of *_count

		 return (double)nSecond + (((double)nCycleCount+((double)nCycleOffset/3072.0))/8000.0);
	}
	*/

	//std::cout << "seconds: " << seconds << " cycles: " << cycles << " clocks: " << clocks << std::endl;
	frametime = (unsigned long long int)( (double)((seconds * 8000 + cycles) * 3072 + clocks) * 40.69 );

	return ++framecount;
}


void DCImageSource::getImage( RGBImage&       target ) const { }

void DCImageSource::getImage( IntensityImage& target ) const {
	//dc1394_YUV411_to_MONO8( rawdata, target.data, width, height );
	memcpy( target.data, rawdata, width*height );
	target.timestamp( frametime );
}

void DCImageSource::release( ) {
	if (frame) dc1394_capture_enqueue( camera[num], frame );
}


void DCImageSource::set_feature( dc1394feature_t feature, int value ) {

	if (!(features.feature[feature-DC1394_FEATURE_MIN].available)) return;

	if (value == IMGSRC_OFF) {

		usleep( SAFETY_DELAY ); dc1394_feature_set_power( camera[num], feature, DC1394_OFF );

	} else if (value == IMGSRC_AUTO) {

		usleep( SAFETY_DELAY ); dc1394_feature_set_power( camera[num], feature, DC1394_ON );
		usleep( SAFETY_DELAY ); dc1394_feature_set_mode ( camera[num], feature, DC1394_FEATURE_MODE_AUTO );

	} else {

		int tmp = -1;

		usleep( SAFETY_DELAY ); dc1394_feature_set_power( camera[num], feature, DC1394_ON );
		usleep( SAFETY_DELAY ); dc1394_feature_set_mode ( camera[num], feature, DC1394_FEATURE_MODE_MANUAL );
		usleep( SAFETY_DELAY ); dc1394_feature_set_value( camera[num], feature, value );
		usleep( SAFETY_DELAY ); dc1394_feature_get_value( camera[num], feature, (uint32_t*)(&tmp) );

		if (tmp != value) {
			std::ostringstream msg; msg << "Error: setting camera feature " << feature << " failed.";
			throw std::runtime_error( msg.str() );
		}
	}
}

void DCImageSource::setBrightness( int bright ) { set_feature( DC1394_FEATURE_BRIGHTNESS, bright ); }
void DCImageSource::setExposure  ( int expo   ) { set_feature( DC1394_FEATURE_EXPOSURE,   expo   ); }
void DCImageSource::setShutter   ( int speed  ) { set_feature( DC1394_FEATURE_SHUTTER,    speed  ); }
void DCImageSource::setGain      ( int gain   ) { set_feature( DC1394_FEATURE_GAIN,       gain   ); }

void DCImageSource::setFPS( int fps ) { }

void DCImageSource::printInfo( int par ) {
	dc1394_feature_get_all( camera[num], &features );
	if (!par) dc1394_feature_print_all( &features, stdout );
	else dc1394_feature_print( &(features.feature[par-DC1394_FEATURE_MIN]), stdout );
}


// for some reason, we have to set nibble #6 in the offset..
uint32_t DCImageSource::getReg( uint64_t offset ) {
	uint32_t tmp = 0x12345678;
	usleep( SAFETY_DELAY ); dc1394_get_register( camera[num], offset | 0xF00000, &tmp );
	return tmp;
}

void DCImageSource::setReg( uint64_t offset, uint32_t value ) {
	usleep( SAFETY_DELAY ); dc1394_set_register( camera[num], offset | 0xF00000, value );
}

