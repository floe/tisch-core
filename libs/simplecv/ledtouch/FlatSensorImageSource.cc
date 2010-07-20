#include <stdio.h>
#include "FlatSensorImageSource.h"


using namespace std;

FlatSensorImageSource::FlatSensorImageSource(int width, int height, char* filename,bool normIt) : ImageSource()
{
	m_pBT = new BigTouch();
	BigTouch::ERR err = BigTouch::ERR_NO_ERROR;
	
	if ((err = m_pBT->create(filename))!=BigTouch::ERR_NO_ERROR)
	{ 
		cout<<m_pBT->getErrString(err)<<endl;
		throw std::runtime_error( "Error connecting BigTouch!" );
	}

	m_normIt = normIt;
	m_output_width = width;
	m_output_height = height;
	m_bt_width = m_pBT->getWidth();
	m_bt_height = m_pBT->getHeight();

	m_imTmp = cvCreateImage(cvSize(m_bt_width,m_bt_height), IPL_DEPTH_8U, 1);
	m_im = cvCreateImage(cvSize(m_output_width,m_output_height), IPL_DEPTH_8U, 1);

	m_pImgData = (float*) malloc(m_bt_width*m_bt_height*sizeof(float));
	
	if (m_normIt)
		m_pNorm = new ItNorm(48,32);
	else
		m_pNorm = 0;	
	
	m_initialized = true;	
}

FlatSensorImageSource::~FlatSensorImageSource()
{
	if (m_initialized)
	{
		cvReleaseImage(&m_imTmp);
		cvReleaseImage(&m_im);
		free(m_pImgData);
		delete m_pBT; m_pBT=0;
	}
	m_initialized = false;
}


int FlatSensorImageSource::acquire()
{
	m_pBT->getFilteredData(m_pImgData); // 48*32px img data; already unshuffled, filtered, scaled, whatever :-)

	if (m_pNorm && m_normIt)
		m_pNorm->iterativeNormalize(m_pImgData,m_pImgData,5, 0, 10.0f);

	for (int i=0;i<m_bt_width*m_bt_height;i++)
	{
		int val = (int) (m_pImgData[i] * 255.0f+0.5f); 
		val = (val>255)?255:val;
		val = (val<0)?0:val;
		m_imTmp->imageData[i] = val;
	} 
	cvResize(m_imTmp, m_im, CV_INTER_CUBIC);

	return 1; // unknown return ?!
}

void FlatSensorImageSource::release() 
{
// nothing to do here ...
}

void FlatSensorImageSource::start()
{
	m_pBT->startThread();
}

void FlatSensorImageSource::stop()
{
	m_pBT->stopThread();
}

void FlatSensorImageSource::getImage( IntensityImage& target ) const
{
	memcpy( target.getData(), m_im->imageData, m_output_width*m_output_height );
}

void FlatSensorImageSource::getImage( RGBImage&       target ) const
{
}
