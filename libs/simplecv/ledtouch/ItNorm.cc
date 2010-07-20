#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ItNorm.h"

#define MAX(a,b)	((a>b)?a:b)
#define MIN(a,b)	((a<b)?a:b)
#define ABS(a)		((a<0)?-a:a)

#define PI 	3.14159265358979323846

ItNorm::ItNorm(int w, int h) /*: m_width(w), m_height(h) gibt merkwürdige meldung */
{
	m_width =  w;
	m_height = h;	
	createITKernel();	

	m_tmp1 = (float*) malloc(m_width*m_height*sizeof(float));
	m_imgExc = (float*) malloc(m_width*m_height*sizeof(float));
	m_imgInh = (float*) malloc(m_width*m_height*sizeof(float));
}

ItNorm::~ItNorm()
{
	free(m_gExc);
	free(m_gInh);
	free(m_tmp1);
	free(m_imgExc);
	free(m_imgInh);		
}

void ItNorm::calcMinMax(float* src, float* min, float* max)
{
	
	float _min = *src;
	float _max = *src;
	
	for (int i=0;i<m_width*m_height;i++)
	{
		_min = (src[i]<_min)?src[i]:_min;
		_max = (src[i]>_max)?src[i]:_max;
	}	
	*min = _min;
	*max = _max;
}


void ItNorm::normalizeImg(float* img, float rangeMin, float rangeMax)
{
	float foundmax = 0.0f;
	float foundmin = 0.0;
	
	calcMinMax(img,&foundmin,&foundmax);
	
	for (int i=0;i<m_width*m_height;i++)
	{
		float val = img[i];
		
		if ((foundmax == 0) && (foundmin==0))
			img[i] = val;
		else
		{
			if (foundmin == foundmax)
				img[i] = val - foundmax + 0.5f*(rangeMin + rangeMax);
			else
				img[i] = (val - foundmin) / (foundmax - foundmin) * ABS(rangeMax-rangeMin) + rangeMin;
		}
	}
}







float* ItNorm::gaussian(float peak, float sigma, int maxhw, int* datasize)
{
	float x = sigma * (float) sqrt(-2.0 * log(1.0/100.0));
	int hw = (int) floor(x);
	
	if ((maxhw > 0) & (hw > maxhw)) 
		hw = maxhw; 
	
	if (peak == 0) 
		peak = 1.0f / (sigma * sqrt(2.0*PI)); 
	
//	float sig22 = -0.5f / (sigma * sigma);
	
//	printf("%d\n",hw);
	
	*datasize = hw*2+1;
	float* data = (float*) malloc((hw*2+1)*sizeof(float));
	float* wptr = data;
	for (int i=-hw;i<=hw;i++)
	{
		*wptr = peak * exp(-((float) (i*i)) / (2.0f*sigma*sigma));
//		printf("%d - %.6f\n",i,*wptr);
		wptr++;
	}
	return data;
}

void ItNorm::createITKernel()
{
	// normalize [0...10]
	int sz = MAX(m_width,m_height);
	int maxhw = MAX(0,MIN(m_width,m_height)/2)-1;
	float esig = (float) sz * ITEREXSIG * 0.01;
	float isig = (float) sz * ITERINSIG * 0.01;
	
	m_gExcSize=0;
	m_gInhSize=0;
	m_gExc = gaussian(ITERCOEX/(esig*sqrt(2.0*PI)),esig,maxhw, &m_gExcSize);
	m_gInh = gaussian(ITERCOIN/(isig*sqrt(2.0*PI)),isig,maxhw, &m_gInhSize);
	
	float sumE = 0.0f;
	float sumI = 0.0f;
	for (int k=0;k<m_gExcSize;k++)
	{
//		printf("%d: %.3f\n",k,gExc[k]);
		sumE+=m_gExc[k];
	}
	
	for (int k=0;k<m_gInhSize;k++)
	{
//		printf("%d: %.3f\n",k,gInh[k]);
		sumI+=m_gInh[k];
	}
	m_gExcSum = sumE;
	m_gInhSum = sumI;
//	printf("sumexc %.3f, suminh %.3f\n",sumE,sumI);

	printf("ExcSize: %d, InhSize: %d\n",m_gExcSize,m_gInhSize);

}

void ItNorm::clampZero(float* img, float* dst)
{
	for (int i=0;i<m_width*m_height;i++)
		dst[i] = MAX(0.0f,img[i]);
}

void ItNorm::convoluteX(float* kernel, int ksize, float ksum, float* src, float* dst)
{
	int kernelw = ksize >> 1;
	for (int y=0;y<m_height;y++)
	{
		for (int x=0;x<m_width;x++)
		{
			int start = MAX(x-kernelw,0)-x;	// sollte negativ sein
			int end = MIN(x+kernelw,m_width-1)-x;	// sollte positiv sein

			float sum = 0.0f;
			float con = 0.0f;

			for (int j=start;j<=end;j++)
			{
				float k = kernel[j+kernelw];
				sum += k;
				con += k*src[(x+j)+y*m_width];
			}
			dst[x+y*m_width] = con * ksum / sum; 	// sum kann nie null sein!
		}
	}
}

void ItNorm::convoluteY(float* kernel, int ksize, float ksum, float* src, float* dst)
{
	int kernelw = ksize >> 1;
	for (int y=0;y<m_height;y++)
	{
		for (int x=0;x<m_width;x++)
		{
			int start = MAX(y-kernelw,0)-y;	// sollte negativ sein
			int end = MIN(y+kernelw,m_height-1)-y;	// sollte positiv sein

			float sum = 0.0f;
			float con = 0.0f;

			for (int j=start;j<=end;j++)
			{
				float k = kernel[j+kernelw];
				sum += k;
				con += k*src[(y+j)*m_width + x];
			}
			dst[x+y*m_width] = con * ksum / sum; 	// sum kann nie null sein!
		}
	}
	
}


void ItNorm::calcDiff(float* src, float* dst, float* imgInh, float* imgExc)
{
	float min=0;
	float max=0;
	
	calcMinMax(src,&min,&max);
	
	float globInhi = 0.01f * ITERINHI * max;
	
	for (int i=0;i<m_width*m_height;i++)
	{
		float np = MAX(src[i] + imgExc[i] - imgInh[i] - globInhi,0.0f);	// inklusive clamping!
		dst[i] = np;		
	}	
}

void ItNorm::iterativeNormalize(float* img, float* dst, int numIter, float min, float max)
{
	clampZero(img, dst);

/*  	if (min || max)
		normalizeImg(dst, min,max);*/

	for (int i=0;i<numIter;i++)
	{
		convoluteX(m_gExc, m_gExcSize, m_gExcSum, dst,m_tmp1);
		convoluteY(m_gExc, m_gExcSize, m_gExcSum, m_tmp1,m_imgExc);
		convoluteX(m_gInh, m_gInhSize, m_gInhSum, dst,m_tmp1);
		convoluteY(m_gInh, m_gInhSize, m_gInhSum, m_tmp1,m_imgInh);
		calcDiff(dst,dst,m_imgInh,m_imgExc);
	}
//	normalizeImg(dst, 0.0f, 1.0f);
}
