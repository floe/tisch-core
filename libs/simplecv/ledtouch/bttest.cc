/*-----------------------------------------------------------------------------
	Bigtouch Test-Tool
-----------------------------------------------------------------------------*/

#include <vector>
#include <sys/time.h>
#include <time.h>
using namespace std;

#include "IntensityImage.h"
#include "FlatSensorImageSource.h"

int main()
{
	struct timeval tv1, tv2;
	struct timezone tz1, tz2;
	long totalT;

	IntensityImage im(640,427);
	IplImage* imgScaled = cvCreateImage(cvSize(640,427), IPL_DEPTH_8U, 1);
	IplImage* imgData = cvCreateImage(cvSize(48,32), IPL_DEPTH_8U, 1);

	FlatSensorImageSource* pFL = new FlatSensorImageSource(640,427,(char*) "bigtouch.bin",false);
	
	cout<<"connected!"<<endl;

	pFL->start();
	
	BigTouch* pBT = pFL->getBigTouch();
	
	float data[48*32];
	float dataold[48*32];
//	float datanew[48*32];
	
	ItNorm* pN = new ItNorm(48,32);	
	
//	usleep(100000);
	cvNamedWindow("scaled", CV_WINDOW_AUTOSIZE);
	gettimeofday(&tv1, &tz1);
	long sum=0;
	long num=0;
	for (;;)
	{
		gettimeofday(&tv2, &tz2);
		totalT = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);	
//		printf("%u\n",totalT);
		sum+=totalT;
		num++;
//		printf("%u \n",sum/num);
		memcpy(&tv1,&tv2,sizeof(tv1));

		while (!pBT->newData()) { usleep(1000); }	

		pFL->acquire();
		pFL->getImage( im );

		pBT->getFilteredData(data); 
		
		memcpy(imgScaled->imageData, im.getData(), 640*480);	
		cvShowImage("scaled",imgScaled);
		int key = cvWaitKey(10);
		if (key!=-1)
			break;
	}

	pN->iterativeNormalize(data,data,5, 0, 10.0f);
	
	for (int i=0;i<48*32;i++)
	{
		int val = (int) (data[i] * 255.0f+0.5f); 
		val = (val>255)?255:val;
		val = (val<0)?0:val;
		imgData->imageData[i] = val;
	} 
		
	cvSaveImage( "imgscaled.bmp",imgScaled );	
	cvSaveImage( "imgreal.bmp",imgData );	
	cvDestroyAllWindows();
	cvReleaseImage(&imgScaled);	

	pFL->stop();
	
	delete (pFL);
	return 0;
}
