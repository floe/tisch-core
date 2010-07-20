#ifndef _FLATSENSORIMAGESOURCE_H_
#define _FLATSENSORIMAGESOURCE_H_

#undef B
#include <cv.h>
#include <highgui.h>


#include "../ImageSource.h"
#include "../RGBImage.h"
#include "../IntensityImage.h"
#include "../Image.h"
#include "BigTouch.h"
#include "ItNorm.h"

//class /*TISCH_SHARED*/ FlatSensorImageSource: public ImageSource {
class FlatSensorImageSource: public ImageSource {

	public:

		FlatSensorImageSource(int width, int height, char* filename, bool normIt=false);
		virtual ~FlatSensorImageSource();

		virtual int  acquire();
		virtual void release();

		virtual void start();
		virtual void stop ();
		
		virtual void getImage( IntensityImage& target ) const;
		virtual void getImage( RGBImage&       target ) const;

		virtual void setGain( int gain ) {}
		virtual void setExposure( int exp ) {}
		virtual void setShutter( int speed ) {}
		virtual void setBrightness( int bright ) {}

		virtual void setFPS( int fps ) {}

		virtual void printInfo( int feature = 0 ) {}
		
// bypass FlatSensorImageSource		
		BigTouch*	getBigTouch() { return m_pBT; }

		

	protected:	
		BigTouch*		m_pBT;
		bool			m_initialized;
		int				m_output_width;
		int				m_output_height;
		int				m_bt_width;
		int				m_bt_height;
		u16*			m_pBuf;
		IplImage*		m_im;
		IplImage*		m_imTmp;
		

		float*			m_pImgData;
		
		ItNorm*			m_pNorm;
		bool			m_normIt;
		
};

#endif // _IMAGESOURCE_H_

