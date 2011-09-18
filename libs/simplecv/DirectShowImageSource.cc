/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "DirectShowImageSource.h"

#include <d3d9.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stdexcept>

#define LOG4CPP_INFO(x,y) x << y << std::endl;
#define LOG4CPP_WARN(x,y) x << y << std::endl;
#define LOG4CPP_DEBUG(x,y) x << y << std::endl;


DirectShowImageSource::DirectShowImageSource( int dwidth, int dheight, const char* videodev, int verbose ) {

	if ( FAILED( CoInitializeEx( NULL, COINIT_MULTITHREADED ) ) )
		throw std::runtime_error( "Error in CoInitializeEx" );

	running = 0;
	devname = videodev;
	width   = dwidth;
	height  = dheight;

	imgbuffer = 0;

	initGraph();
	Sleep(10);
	start();
}


DirectShowImageSource::~DirectShowImageSource(){
	delete imgbuffer;
	if ( m_pMediaControl )
		m_pMediaControl->Stop();
}


void DirectShowImageSource::start() {
	if ( !running && m_pMediaControl )
		m_pMediaControl->Run();
	running = 1;
}


void DirectShowImageSource::stop() {
	if ( running && m_pMediaControl )
		m_pMediaControl->Pause();
	running = 0;
}


void DirectShowImageSource::initGraph() {

	// Create the System Device Enumerator.
	AutoComPtr< ICreateDevEnum > pDevEnum;
	AutoComPtr< IEnumMoniker > pEnum;

	HRESULT hr = pDevEnum.CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER );
	if ( SUCCEEDED( hr ) )
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pEnum.p, 0 );

	AutoComPtr< IMoniker > pMoniker;
	AutoComPtr< IMoniker > pSelectedMoniker;
	std::string sSelectedCamera;
	while ( pEnum.p && (pEnum->Next( 1, &pMoniker.p, NULL ) == S_OK) ) {
		if ( !pSelectedMoniker ) pSelectedMoniker = pMoniker;
		AutoComPtr< IPropertyBag > pPropBag;
		hr = pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, (void**)(&pPropBag.p) );
		if ( FAILED( hr ) ) {
			pMoniker.Release();
			continue;  // Skip this one, maybe the next one will work.
		} 

		// Find the description or friendly name.
		VARIANT varName;
		VariantInit( &varName );
		hr = pPropBag->Read( L"Description", &varName, 0 );
		if ( FAILED( hr ) )	hr = pPropBag->Read( L"FriendlyName", &varName, 0 );

		if ( SUCCEEDED( hr ) ) {
			char sName[ 128 ];
			WideCharToMultiByte( CP_ACP, 0, varName.bstrVal, -1, sName, 128, 0, 0 );
			LOG4CPP_INFO( std::cerr, "Possible capture device: " << &sName[0] );

			// select device based on name
			if ( !devname.empty() && strstr( sName, devname.c_str() ) ) {
				sSelectedCamera = sName;
				pSelectedMoniker = pMoniker;
			}

			VariantClear( &varName ); 
		}

		pMoniker.Release();
	}

	// check if a capture device was found
	if ( !pSelectedMoniker ) throw std::runtime_error( "No video capture device found" );

	LOG4CPP_INFO( std::cerr, "Using camera: " << sSelectedCamera );

	// create capture graph
    AutoComPtr< IGraphBuilder > pGraph;
    AutoComPtr< ICaptureGraphBuilder2 > pBuild;

    // Create the Capture Graph Builder.
    hr = pBuild.CoCreateInstance( CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER );
    if ( SUCCEEDED( hr ) ) {
        // Create the Filter Graph Manager.
        hr = pGraph.CoCreateInstance( CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER );
		 // Initialize the Capture Graph Builder.
        if ( SUCCEEDED( hr ) ) pBuild->SetFiltergraph( pGraph );
        else throw std::runtime_error( "Error creating filter graph manager" );
    } else throw std::runtime_error( "Error creating capture graph builder" );

	// create capture device filter
	AutoComPtr< IBaseFilter > pCaptureFilter;
	if ( FAILED( pSelectedMoniker->BindToObject( 0, 0, IID_IBaseFilter, (void**)&pCaptureFilter.p ) ) )
		throw std::runtime_error( "Unable to create capture filter" );

	if ( FAILED( pGraph->AddFilter( pCaptureFilter, L"Capture" ) ) )
		throw std::runtime_error( "Unable to add capture filter" );

	// find output pin for configuration
	AutoComPtr< IPin > pPin;
	if ( FAILED( pBuild->FindPin( pCaptureFilter, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, FALSE, 0, &pPin.p ) ) )
		throw std::runtime_error( "Unable to find pin" );

	// enumerate media types
	AutoComPtr< IAMStreamConfig > pStreamConfig;
	if ( FAILED( pPin.QueryInterface< IAMStreamConfig >( pStreamConfig ) ) )
	{ LOG4CPP_WARN( std::cerr, "Unable to get IAMStreamConfig interface" ); }
	else {
		int iCount, iSize;
		pStreamConfig->GetNumberOfCapabilities( &iCount, &iSize );
		BYTE* buf = new BYTE[ iSize ];

		bool bSet = false;
		for ( int iCap = 0; iCap < iCount; iCap++ ) {
			AM_MEDIA_TYPE *pMediaType;
			pStreamConfig->GetStreamCaps( iCap, &pMediaType, buf );

			if ( pMediaType->majortype != MEDIATYPE_Video || pMediaType->formattype != FORMAT_VideoInfo )
				continue;
			VIDEOINFOHEADER* pInfo = (VIDEOINFOHEADER*)pMediaType->pbFormat;

			LOG4CPP_INFO( std::cerr, "Media type: fps=" << 1e7 / pInfo->AvgTimePerFrame << 
				", width=" << pInfo->bmiHeader.biWidth << ", height=" << pInfo->bmiHeader.biHeight <<
				", type=" << ( pMediaType->subtype == MEDIASUBTYPE_RGB24 ? "RGB24" : "?" ) );

			// set first format with correct size, but prefer RGB24 
			if ( pInfo->bmiHeader.biWidth == width && pInfo->bmiHeader.biHeight == height &&
				( !bSet || pMediaType->subtype == MEDIASUBTYPE_RGB24 ) ) {
				pStreamConfig->SetFormat( pMediaType );
				if ( bSet )	break;
				bSet = true;
			}

			// TODO: DeleteMediaType
		}
	}

	// create sample grabber filter
	AutoComPtr< IBaseFilter > pSampleGrabberFilter;
	if ( FAILED( pSampleGrabberFilter.CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER ) ) )
		throw std::runtime_error( "Unable to create sample grabber filter" );

	if ( FAILED( pGraph->AddFilter( pSampleGrabberFilter, L"SampleGrab" ) ) )
		throw std::runtime_error( "Unable to add sample grabber filter" );

	// configure sample grabber
	AutoComPtr< ISampleGrabber > pSampleGrabber;
	pSampleGrabberFilter.QueryInterface( pSampleGrabber );
	pSampleGrabber->SetOneShot( FALSE );
	pSampleGrabber->SetBufferSamples( FALSE );
	pSampleGrabber->SetCallback( this, 0 ); // 0 = Use the SampleCB callback method.

	// make it picky on media types
	AM_MEDIA_TYPE mediaType;
	memset( &mediaType, 0, sizeof( mediaType ) );
	mediaType.majortype = MEDIATYPE_Video;
	mediaType.subtype = MEDIASUBTYPE_RGB24;
	pSampleGrabber->SetMediaType( &mediaType );

	// null renderer
	AutoComPtr< IBaseFilter > pNullRenderer;
	if ( FAILED( pNullRenderer.CoCreateInstance( CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER ) ) )
		throw std::runtime_error( "Unable to create null renderer filter." );

	if ( FAILED( pGraph->AddFilter( pNullRenderer, L"NullRender" ) ) )
		throw std::runtime_error( "Unable to add null renderer filter." );

	// connect all filters
	hr = pBuild->RenderStream(
		&PIN_CATEGORY_CAPTURE, // Connect this pin ...
		&MEDIATYPE_Video,      // with this media type ...
		pCaptureFilter,        // on this filter ...
		pSampleGrabberFilter,  // to the Sample Grabber ...
		pNullRenderer );       // ... and finally to the Null Renderer.
	if ( FAILED( hr ) )
		throw std::runtime_error( "Unable to render stream." );

	// get media type
	pSampleGrabber->GetConnectedMediaType( &mediaType );
	if ( mediaType.majortype != MEDIATYPE_Video || mediaType.subtype != MEDIASUBTYPE_RGB24 ||
		mediaType.formattype != FORMAT_VideoInfo )
		throw std::runtime_error( "Unsupported MEDIATYPE." );

	VIDEOINFOHEADER* pVidInfo = (VIDEOINFOHEADER*)mediaType.pbFormat;
	m_sampleWidth = pVidInfo->bmiHeader.biWidth;
	m_sampleHeight = pVidInfo->bmiHeader.biHeight;
	LOG4CPP_INFO( std::cerr, "Image dimensions: " << m_sampleWidth << "x" << m_sampleHeight );
	// TODO: FreeMediaType( &mediaType );

	// start stream
	pGraph.QueryInterface< IMediaControl >( m_pMediaControl );
	m_pMediaControl->Pause();
}

void DirectShowImageSource::getImage( IntensityImage& target ) const {
	if (imgbuffer) imgbuffer->getIntensity( target );
}

void DirectShowImageSource::getImage( RGBImage&       target ) const {
	if (imgbuffer) target = *imgbuffer;
}


STDMETHODIMP DirectShowImageSource::SampleCB( double Time, IMediaSample *pSample ) {

	// TODO: check for double frames when using multiple cameras...
	//LOG4CPP_DEBUG( std::cerr, "SampleCB called" );

	if ( !running ) return S_OK;

	if ( pSample->GetSize() < m_sampleWidth * m_sampleHeight * 3 ) {
		LOG4CPP_INFO( std::cerr, "Invalid sample size" );
		return S_OK;
	}

	BYTE* pBuffer;
	if ( FAILED( pSample->GetPointer( &pBuffer ) ) ) {
		LOG4CPP_INFO( std::cerr, "GetPointer failed" );
		return S_OK;
	}

	if (!imgbuffer) imgbuffer = new RGBImage( m_sampleWidth, m_sampleHeight );
	memcpy( imgbuffer->getData(), pBuffer, pSample->GetSize() );

	return S_OK;
}


// ISampleGrabberCB: fake reference counting.
STDMETHODIMP_(ULONG) DirectShowImageSource::AddRef()  { return 1; }
STDMETHODIMP_(ULONG) DirectShowImageSource::Release() { return 2; }

STDMETHODIMP DirectShowImageSource::QueryInterface( REFIID riid, void** ppvObject ) {
	if ( NULL == ppvObject ) return E_POINTER;
	if ( riid == __uuidof( IUnknown ) ) {
		*ppvObject = static_cast<IUnknown*>( this );
		return S_OK;
	}
	if ( riid == __uuidof( ISampleGrabberCB ) ) {
		*ppvObject = static_cast<ISampleGrabberCB*>( this );
		return S_OK;
	}
	return E_NOTIMPL;
}

STDMETHODIMP DirectShowImageSource::BufferCB( double Time, BYTE* pBuffer, long BufferLen ) {
	std::cerr << "BufferCB called" << std::endl;
	return E_NOTIMPL;
}


// dummy functions for now
void DirectShowImageSource::setFPS( int fps ) {}
void DirectShowImageSource::setGain( int gain ) {}
void DirectShowImageSource::setExposure( int exp ) {}
void DirectShowImageSource::setShutter( int speed ) {}
void DirectShowImageSource::setBrightness( int bright ) {}
void DirectShowImageSource::printInfo( int feature ) {}

int  DirectShowImageSource::acquire( ) { return 1; }
void DirectShowImageSource::release( ) {}