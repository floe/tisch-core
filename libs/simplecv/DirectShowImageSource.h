/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _DIRECTSHOWIMAGESOURCE_H_
#define _DIRECTSHOWIMAGESOURCE_H_

// These classes are based on the DirectShowFrameGrabber class from the
// Ubitrack library, written by Daniel Pustka under LGPL. Thanks, Daniel!

#include <iostream>
#include <dshow.h>

#include "DirectShowStub.h"
#include "ImageSource.h"
#include "RGBImage.h"


// dirt-cheap replacement for CComPtr, which is not included in the free windows compilers
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


class TISCH_SHARED DirectShowImageSource: protected ISampleGrabberCB, public ImageSource {

	public:

		DirectShowImageSource( int dwidth, int dheight, const char* videodev = "", int verbose = 0 );
		~DirectShowImageSource( );

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

		// initializes the direct show filter graph
		void initGraph();

		// width & height of resulting image
		int m_sampleWidth;
		int m_sampleHeight;

		// desired camera name
		std::string devname;

		RGBImage* imgbuffer;
		int running;

		// pointer to DirectShow filter graph
		AutoComPtr< IMediaControl > m_pMediaControl;

		// ISampleGrabberCB: fake reference counting.
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		STDMETHODIMP QueryInterface( REFIID riid, void** ppvObject );

		STDMETHODIMP SampleCB( double Time, IMediaSample* pSample );

		STDMETHODIMP BufferCB( double Time, BYTE* pBuffer, long BufferLen );
};

#endif // _DIRECTSHOWIMAGESOURCE_H_
