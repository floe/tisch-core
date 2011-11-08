/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  // ioctl
#include <unistd.h> // close
#include <string.h> // memset
#include <error.h>  // errno
#include <errno.h>  // errno

#include <sys/time.h>
#include <asm/types.h>

#include <cstdio>
#include <iostream> // cout
#include <string>

#include <linux/videodev2.h>


#include "V4LImageSource.h"
#include "YUV420Image.h"
#include "YUYVImage.h"
#include "ColorLUT.h"

#ifdef HAS_LIBV4L
	#define ioctl v4l2_ioctl
	#define close v4l2_close
	#define open  v4l2_open
#endif


int formats[] = {
	V4L2_PIX_FMT_YUV420,
	V4L2_PIX_FMT_YUYV
};

const int num_formats = sizeof(formats)/sizeof(int);

inline int safe_ioctl( int dev, int type, void* args ) {
	usleep( 500 );
	return ioctl( dev, type, args );
}


V4LImageSource::V4LImageSource( const std::string& path, int _width, int _height, int fps, int debug ) {

  struct v4l2_requestbuffers vreq;
	struct v4l2_capability     vcap;
	struct v4l2_cropcap        ccap;
	struct v4l2_format         vfmt;
	struct v4l2_crop           crop;

	double bpp;
	int width, height;
	int ret = -1;

	//v4l2_log_file = fopen("v4l2.log","a+");

	// clear all structs
	memset( &vreq, 0, sizeof(vreq) );
	memset( &vcap, 0, sizeof(vcap) );
	memset( &ccap, 0, sizeof(ccap) );
	memset( &vfmt, 0, sizeof(vfmt) );
	memset( &crop, 0, sizeof(crop) );
	
	// open the device
	vdev = open( path.c_str(), O_RDWR );
	if (vdev < 0) throw std::runtime_error( std::string("open(") + path + "): " + std::string(strerror(errno)) );

	// query capabilities
	if ( ioctl( vdev, VIDIOC_QUERYCAP, &vcap ) < 0 ) throw std::runtime_error( "VIDIOC_QUERYCAP" );

	// query and disable cropping
	ccap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if ( ioctl( vdev, VIDIOC_CROPCAP, &ccap ) == 0 ) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = ccap.defrect;
		ioctl( vdev, VIDIOC_S_CROP, &crop );
	}

	// image format
	vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vfmt.fmt.pix.field  = V4L2_FIELD_ANY;
	vfmt.fmt.pix.width  = _width;
	vfmt.fmt.pix.height = _height;

	// query available formats
	for ( int i = 0; i < num_formats; i++ ) {
		vfmt.fmt.pix.pixelformat = formats[i];
		ret = ioctl( vdev, VIDIOC_S_FMT, &vfmt );
		if (ret == 0) break;
	}
	if (ret == -1) throw std::runtime_error( "VIDIOC_S_FMT" );

	width  = vfmt.fmt.pix.width;
	height = vfmt.fmt.pix.height;
	format = vfmt.fmt.pix.pixelformat;

	switch (format) {
		case V4L2_PIX_FMT_YUV420: bpp = 1.5; break;
		case V4L2_PIX_FMT_YUYV:   bpp = 2.0; break;
		default: throw std::runtime_error( "V4LImageSource: unsupported raw image format" );
	}

	// request buffers
	vreq.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vreq.count  = IMGSRC_BUFCOUNT;
	vreq.memory = V4L2_MEMORY_MMAP;

	if ( ioctl( vdev, VIDIOC_REQBUFS, &vreq ) < 0 ) throw std::runtime_error( std::string("VIDIOC_REQBUFS: ") + std::string(strerror(errno)) );
	bufcount = vreq.count;
	if (debug) std::cerr << "V4LImageSource: reserved " << bufcount << " buffers for mmap.\n";

	// setup mmap
	for ( int i = 0; i < bufcount; i++ ) {
		struct v4l2_buffer tmpbuf;
		buf_ioctl( &tmpbuf, VIDIOC_QUERYBUF, i );
		//if ( (width*height*bpp) > (int)tmpbuf.length ) throw std::runtime_error( std::string("V4LImageSource: buffer size mismatch") );
		buffers[i] = new Image( width, height, bpp, vdev, (unsigned long long)(&tmpbuf) );
	}

	// final setup and start
	setFPS( fps );
	current = -1;
	start();

	// done, do debug output
	if (debug) 
		std::cerr << "V4LImageSource: created for "
		          << (char)((format & 0x000000FF) >>  0)
		         	<< (char)((format & 0x0000FF00) >>  8)
		         	<< (char)((format & 0x00FF0000) >> 16)
		         	<< (char)((format & 0xFF000000) >> 24)
		          << '@' << width << 'x' << height << "\n";

	// initialize YUV-RGB lookup table
	initLUT();
}


V4LImageSource::~V4LImageSource() {
	stop();
	for ( int i = 0; i < bufcount; i++) delete buffers[i];
	close( vdev );
	freeLUT();
}


void V4LImageSource::start() {

	// throw all our buffers into the queue
	struct v4l2_buffer tmpbuf;
	for ( int i = 0; i < bufcount; i++ ) buf_ioctl( &tmpbuf, VIDIOC_QBUF, i );

	// start the stream
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if ( safe_ioctl ( vdev, VIDIOC_STREAMON, &type ) < 0 ) throw std::runtime_error( std::string("VIDIOC_STREAMON: ") + std::string(strerror(errno)) );
}

void V4LImageSource::stop() {
	// stop the stream
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if ( safe_ioctl ( vdev, VIDIOC_STREAMOFF, &type ) < 0 ) throw std::runtime_error( std::string("VIDIOC_STREAMOFF: ") + std::string(strerror(errno)) );
}


int V4LImageSource::acquire() {
	struct v4l2_buffer tmpbuf;
	buf_ioctl( &tmpbuf, VIDIOC_DQBUF, 0, "VIDIOC_DQBUF" );
	current = tmpbuf.index;
	frametime = tmpbuf.timestamp.tv_sec * 1000000000 + tmpbuf.timestamp.tv_usec * 1000;
	return tmpbuf.sequence;
	/*oldt = newt;
	newt = tmpbuf.timestamp;
	long difference = newt.tv_sec - oldt.tv_sec;
	if (difference == 0) { 
		difference = newt.tv_usec - oldt.tv_usec;
	} else {
		difference = (1000000 - oldt.tv_usec) + newt.tv_usec;
	}
	std::cout << "time difference: " << difference << " usec, frame number #" << tmpbuf.sequence << std::endl;*/
}

void V4LImageSource::release() {
	if (current == -1) return;
	struct v4l2_buffer tmpbuf;
	buf_ioctl( &tmpbuf, VIDIOC_QBUF, current, "VIDIOC_QBUF" );
	current = -1;
}


void V4LImageSource::getImage( IntensityImage& target ) const {
	YUV420Image* raw1;
	YUYVImage*   raw2;
	switch (format) {
		case V4L2_PIX_FMT_YUV420: raw1 = (YUV420Image*)(buffers[current]); raw1->getImage(target); break;
		case V4L2_PIX_FMT_YUYV:   raw2 = (YUYVImage*  )(buffers[current]); raw2->getImage(target); break;
	}
	target.timestamp( frametime );
}

void V4LImageSource::getImage( RGBImage& target ) const {
	YUV420Image* raw1;
	YUYVImage*   raw2;
	switch (format) {
		case V4L2_PIX_FMT_YUV420: raw1 = (YUV420Image*)(buffers[current]); raw1->getImage(target); break;
		case V4L2_PIX_FMT_YUYV:   raw2 = (YUYVImage*  )(buffers[current]); raw2->getImage(target); break;
	}
	target.timestamp( frametime );
}


void V4LImageSource::setGain( int gain ) {

	struct v4l2_control ctrl;

	ctrl.id    = gain ? V4L2_CID_GAIN : V4L2_CID_AUTOGAIN;
	ctrl.value = gain ?          gain :                 1;

	if (safe_ioctl( vdev, VIDIOC_S_CTRL, &ctrl ) < 0) std::cerr << "V4L2_CID_(AUTO)GAIN\n";
}

void V4LImageSource::setExposure( int exp ) {

	struct v4l2_control ctrl;

	ctrl.id    = V4L2_CID_EXPOSURE_AUTO;
	ctrl.value = (exp < 0) ? 2 : 1; // 2 = auto, 1 = manual (at least for UVC cams)

	if (safe_ioctl( vdev, VIDIOC_S_CTRL, &ctrl ) < 0) std::cerr << "V4L2_CID_EXPOSURE_AUTO\n";

	ctrl.id    = V4L2_CID_EXPOSURE_ABSOLUTE;
	ctrl.value = exp;

	if (safe_ioctl( vdev, VIDIOC_S_CTRL, &ctrl ) < 0) std::cerr << "V4L2_CID_(AUTO)GAIN\n";
}

void V4LImageSource::setShutter( int speed ) {
	struct v4l2_control ctrl = { V4L2_CID_EXPOSURE, speed };
	if (safe_ioctl( vdev, VIDIOC_S_CTRL, &ctrl ) < 0) std::cerr << "V4L2_CID_EXPOSURE\n";
}


void V4LImageSource::setBrightness( int bright ) { }

#define do_query(x) query.id = x; \
	if (safe_ioctl( vdev, VIDIOC_QUERYCTRL, &query ) < 0) std::cerr << #x " VIDIOC_QUERYCTRL\n"; else \
	std::cout << query.name << ": range " << query.minimum << "-" << query.maximum << ", step " << query.step << std::endl;


void V4LImageSource::printInfo( int feature ) {
	struct v4l2_queryctrl query;
	do_query(V4L2_CID_EXPOSURE_ABSOLUTE);
	do_query(V4L2_CID_EXPOSURE_AUTO);
	do_query(V4L2_CID_EXPOSURE);
	do_query(V4L2_CID_AUTOGAIN);
	do_query(V4L2_CID_GAIN);
}


void V4LImageSource::setFPS( int fps ) {

	struct v4l2_streamparm setfps;  
	memset( &setfps, 0, sizeof(setfps) );

	setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	setfps.parm.capture.timeperframe.numerator = 1;
	setfps.parm.capture.timeperframe.denominator = fps;

	if (safe_ioctl( vdev, VIDIOC_S_PARM, &setfps ) < 0) std::cerr << "Warning: VIDIOC_S_PARM failed." << std::endl;
}


// ioctl helper function
void V4LImageSource::buf_ioctl( struct v4l2_buffer* buf, int num, int index, const char* name ) {
	memset( buf, 0, sizeof(*buf) );
	buf->type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf->memory = V4L2_MEMORY_MMAP;
	buf->index  = index;
	if ( safe_ioctl( vdev, num, buf ) < 0 ) {
		if (name) fprintf( stderr, "error in ioctl: %s\n", name );
		// TODO: need better error handling here
		// throw std::runtime_error( std::string("error: ") + std::string(strerror(errno)) );
	}
}

