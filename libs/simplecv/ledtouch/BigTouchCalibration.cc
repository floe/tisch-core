/*-----------------------------------------------------------------------------
	Bigtouch Calibration Tool
-----------------------------------------------------------------------------*/
#include <cv.h>
#include <highgui.h>
#include <stdio.h>

#include "IntensityImage.h"
#include "FlatSensorImageSource.h"

#define NUMPANELS	6
#define THRESHOLD	0.7


/*-----------------------------------------------------------------------------
	Calibration with successive approximation 
-----------------------------------------------------------------------------*/
bool doCalibration(u16* cal, float* filtered)
{
	static int step = 256;	// don't change this value
	
	for (int i=0;i<256*NUMPANELS;i++)
	{
		int j = (i+(256*NUMPANELS-NUMPANELS))%(256*NUMPANELS); 

		if (filtered[i]>THRESHOLD)
			cal[j] += step >> 1; // and don't touch this
		else
			cal[j] -= step >> 1; // and this ;-)
	}
	step >>=1;
	if (!step)
		return false;

	return true;	
}	

/*-----------------------------------------------------------------------------
	Strong low-pass filter with saturation 
-----------------------------------------------------------------------------*/
void filterData(u16* img, float* filtered)
{
	for (int i=0;i<256*NUMPANELS;i++)
	{
		filtered[i]=0.9*filtered[i]+0.1*((float) img[i]/65535.0);	// strong lowpass
		if (filtered[i]>1.0)	// saturate
			filtered[i]=1.0;
		if (filtered[i]<0.0)  // saturate
			filtered[i]=0.0;
	}
}	

/*-----------------------------------------------------------------------------
	Gauss-Convolution with seperated kernel
	Border will be zeroed
-----------------------------------------------------------------------------*/
void convolute(int w, int h, float* img, int num)
{
	float g[h][w];
	float tmp[h][w];
	
	memcpy(tmp,img,sizeof(tmp));
	
	for (int i=0;i<num;i++)
	{	
		for (int y=1;y<h-1;y++)
		{
			for (int x=1;x<w-1;x++)
			{
				g[y][x]=1.0*tmp[y][x-1]+2.0*tmp[y][x]+1.0*tmp[y][x+1];	
			}
		}
	
		for (int x=0;x<w;x++)
		{
			for (int y=0;y<h;y++)
			{
				if (!x || !y || (x==(w-1)) || (y==(h-1)))
					tmp[y][x] = 0.0f;
				else
				{
					tmp[y][x]=1.0f*g[y-1][x]+2.0f*g[y][x]+1.0f*g[y+1][x];	
					tmp[y][x]/=16.0f;
				}
			}
		}
	}
	memcpy(img,tmp,sizeof(tmp));	
}

/*-----------------------------------------------------------------------------
	Main 
-----------------------------------------------------------------------------*/
int main()
{
	bool once = true;
	
	cout<<"BigTouch calibration tool V0.1-alpha2-beta8 ;-)"<<endl;
	 
	FlatSensorImageSource* pFL = new FlatSensorImageSource(640,480, (char*) "",false);
	BigTouch* pBT = pFL->getBigTouch();

// at this point already connected with BigTouch	
	cout<<"connected!"<<endl;
	
	int btWidth = pBT->getWidth();
	int btHeight = pBT->getHeight();
	
// data from and for BigTouch
	u16* pCalData = (u16*) malloc(btWidth*btHeight*sizeof(u16));
	u16* pRawData = (u16*) malloc(btWidth*btHeight*sizeof(u16));
	u16* pImgData = (u16*) malloc(btWidth*btHeight*sizeof(u16));

// will be filtered slightly
	float* pFloatImg = (float*) malloc(btWidth*btHeight*sizeof(float));

// used for min/max scaling
	float* pMin = (float*) malloc(btWidth*btHeight*sizeof(float));
	float* pMax = (float*) malloc(btWidth*btHeight*sizeof(float));
	float* pScaled = (float*) malloc(btWidth*btHeight*sizeof(float));

// will be filtered strongly for calibration	
	float* pFilteredData = (float*) malloc(btWidth*btHeight*sizeof(float));

// preset everything with 0
	memset(pCalData,0,btWidth*btHeight*sizeof(u16));
	memset(pRawData,0,btWidth*btHeight*sizeof(u16));
	memset(pImgData,0,btWidth*btHeight*sizeof(u16));
	memset(pMin,0,btWidth*btHeight*sizeof(float));
	memset(pMax,0,btWidth*btHeight*sizeof(float));
	memset(pFloatImg,0,btWidth*btHeight*sizeof(float));
	memset(pScaled,0,btWidth*btHeight*sizeof(float));

// write calibration
	pBT->writeCalRAM((u8*)pCalData);

// some pictures for output	
	IplImage* imgSmall = cvCreateImage(cvSize(btWidth,btHeight), IPL_DEPTH_8U, 1);
	IplImage* imgResized = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);

	IplImage* imgScaledSmall = cvCreateImage(cvSize(btWidth,btHeight), IPL_DEPTH_8U, 1);
	IplImage* imgScaled = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);

// kick BigTouch in the ass to do something :-)
	pBT->startThread();
	
// some windows 
	cvNamedWindow("scaled", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("bild", CV_WINDOW_AUTOSIZE);
	
	cvMoveWindow( "scaled", 0,480+100);		
	cvMoveWindow( "bild", 0,50);


// variables for calibration
	unsigned int frame = 0;			// framecounter
	bool calActive = false;			// calibration active?
	bool scaleActive = false;		// scaling active?

/*	char buf[1024]={0};
	int step=0;
*/
	while (1)
	{
// wait for new data
		while (!pBT->newData()) { usleep(1000); }

// get new data		
		pBT->getRawData(pRawData);	// unshuffled
		pBT->getImgData(pImgData);	// shuffled

// filter data for calibration
// calibration will be performed on unshuffled data ... much easier :-)
		filterData(pRawData, pFilteredData);
		
// check if to calibrate and do it if necessary
		frame++;
		if (frame==10)
		{
			calActive = true;
			cout<<"Calibration active ..."<<endl;
		}
		else if (!(frame % 20/*5*/) && calActive)	// min 20 frames!
		{
			if (!doCalibration(pCalData, pFilteredData))
			{
				calActive = false;
				cout<<"Calibration done."<<endl;
				cout<<"Now calibrate maximum and hit a key when finished"<<endl;
				
				memcpy(pMin,pFloatImg,btWidth*btHeight*sizeof(float));
				memset(pMax,0,btWidth*btHeight*sizeof(float));

				scaleActive = true;
			}
/*			step++;
			sprintf(buf,"cal_step %d.bmp",step);
			for (int i=0;i<btWidth*btHeight;i++)
				imgSmall->imageData[i] = pImgData[i] >> 8;
			cvResize(imgSmall, imgResized, CV_INTER_NN);
			cvSaveImage(buf,imgResized);
*/			
			pBT->writeCalRAM((u8*) pCalData);
			
		}

// copy 16bit data to 8bit - only for displaying RAW data		
		for (int i=0;i<btWidth*btHeight;i++)
			imgSmall->imageData[i] = pImgData[i] >> 8;

		cvResize(imgSmall, imgResized, CV_INTER_NN);

// the scaling part
		for (int i=0;i<btWidth*btHeight;i++)
			pFloatImg[i] = 0.75*pFloatImg[i] + 0.25*((float) pImgData[i] / 65535.0);	// filter!

		if (scaleActive)
		{
			for (int i=0;i<btWidth*btHeight;i++)
			{
				if (pFloatImg[i]>pMax[i])
					pMax[i] = pFloatImg[i];
					
				pScaled[i] = (pFloatImg[i] - pMin[i])/(pMax[i]-pMin[i]+(1.0/65535.0));
			}

			convolute(btWidth, btHeight, pScaled, 1);	// convolute after scaling

			for (int i=0;i<btWidth*btHeight;i++)
			{
				int val = (int) (pScaled[i] * 255.0+0.5);
				val = (val>255)?255:val;
				val = (val<0)?0:val;
				imgScaledSmall->imageData[i] = val;
			} 
		}
		cvResize(imgScaledSmall, imgScaled, CV_INTER_NN/*CUBIC*/);

// display images
		cvShowImage("bild",imgResized);
		cvShowImage("scaled",imgScaled);

		if (once)
		{
			once = false;
			cvWaitKey(10000);
		}
		
// 10ms delay
		int key = cvWaitKey(10);
		if (key!=-1)
			break;
	}
		
// save calibration data
	FILE* pFile;
	pFile = fopen("bigtouch.bin","wb");
	fwrite(pCalData,1,btWidth*btHeight*sizeof(u16),pFile);
	fwrite(pMin,1,btWidth*btHeight*sizeof(float),pFile);
	fwrite(pMax,1,btWidth*btHeight*sizeof(float),pFile);
	fclose(pFile);
	cout<<"Calibration data saved to bigtouch.bin\n"<<endl;
		
	cvDestroyAllWindows();
	cvReleaseImage(&imgSmall);
	cvReleaseImage(&imgResized);
	
	pBT->stopThread();	
	delete (pFL);		// deletes pBT
	
	free(pCalData);
	free(pRawData);
	free(pImgData);
	free(pFloatImg);
	free(pMin);
	free(pMax);
	free(pScaled);
	free(pFilteredData);	
	
	return 0;
}
