/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _FFMVIMAGESOURCE_H_
#define _FFMVIMAGESOURCE_H_

#include <iostream>
//#include <dshow.h>

#include "FlyCapture2.h"
#include "ImageSource.h"
#include "RGBImage.h"

// dirt-cheap replacement for CComPtr, which is not included in the free windows compilers
/*
template< class I > class AutoComPtr {

public:
	
	AutoComPtr(): p( 0 ) { }
	AutoComPtr( I* _p ): p( _p ) { }

	AutoComPtr( const AutoComPtr< I >& o ): p( o.p ) { if ( p ) p->AddRef(); }
	
	~AutoComPtr() { if ( p ) p->Release(); }

	void Release() { if ( p ) p->Release();	p = 0; }

	I* operator->() { return p; }
	operator I*() { return p; }
	
	AutoComPtr< I >& operator=( const AutoComPtr< I >& o ) { 
		Release(); 
		p = o.p; 
		if ( p ) p->AddRef(); 
		return *this;
	}
	
	template< class O >	HRESULT QueryInterface( AutoComPtr< O >& o )
	{ return p->QueryInterface( __uuidof( O ), reinterpret_cast< void** >( &o.p ) ); }
	
	HRESULT CoCreateInstance( REFCLSID rclsid, LPUNKNOWN pUnkOuter = 0, DWORD dwClsContext = CLSCTX_INPROC_SERVER )
	{ return ::CoCreateInstance( rclsid, pUnkOuter, dwClsContext, __uuidof( I ), reinterpret_cast< void** >( &p ) ); }
	
	I* p;
};
*/

class TISCH_SHARED FFMVImageSource: public ImageSource {

	public:

		FFMVImageSource( int dwidth, int dheight, const char* videodev = "", int verbose = 0 );
		~FFMVImageSource( );

		void start();
		void stop();

		void getImage( IntensityImage& target ) const;
		void getImage( RGBImage&       target ) const;

		// dummy functions, currently unimplemented
		void setFPS( int fps );
		void setGain( int gain );
		void setExposure( int exp );
		void setShutter( int speed );
		void setBrightness( int bright );
		void printInfo( int feature = 0 );

		int  acquire();
		void release();

	protected:

		FlyCapture2::Error error;
		FlyCapture2::BusManager busMgr;
		FlyCapture2::Camera*	camera;
		FlyCapture2::Image* image;
		int	microsecondsLastFrame;
		unsigned int secondsLastFrame;
		int	frameCounter;

		void printError(FlyCapture2::Error error);
		void printCameraInfo(FlyCapture2::CameraInfo* pCamInfo);
		void set_feature( FlyCapture2::PropertyType feature, int value );

		// width & height of resulting image
		int m_sampleWidth;
		int m_sampleHeight;

		// desired camera name
		std::string devname;

		RGBImage* imgbuffer;
		IntensityImage* intensityImgbuffer;
		int running;
};

#endif // _FFMVIMAGESOURCE_H_
