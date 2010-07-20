/*-----------------------------------------------------------------------------
	BigTouch
	low-level-communication
-----------------------------------------------------------------------------*/
/*
#include <sys/time.h>
#include <time.h>
*/
#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "BigTouch.h"

#define SAFEFREE(a)	{ if (a) { free(a); a=0;} }
#define SAFEDELETE(a) { if (a) { delete(a); a=0;} }

using namespace std;

sem_t m_sem;
sem_t m_sem_filter;
sem_t semUSB;

/*
long sum=0;
long num=0;
struct timeval tv1, tv2;
struct timezone tz1, tz2;
long totalT;
*/

/*-----------------------------------------------------------------------------
	Constructor
-----------------------------------------------------------------------------*/
BigTouch::BigTouch() : m_pUSB(0), m_numPanels(6), m_numX(3), m_numY(2), m_width(48),
	m_height(32), m_cont(false), m_initialized(0), m_newData(0)
{
	m_pImgData = m_pCalData = 0;
	sem_init(&semUSB, 0, 1);
}

bool BigTouch::loadConfig(char* filename)
{
	FILE* pFile;
	pFile = fopen(filename,"rb");
	
	if (pFile==NULL)
		return false;
	
	int pix = m_width*m_height;
			
	u32 read = fread(m_pCalData,1,pix*sizeof(u16),pFile);
	read += fread(m_pMin,1,pix*sizeof(float),pFile);
	read += fread(m_pMax,1,pix*sizeof(float),pFile);
	
	if (read != (pix * (2*sizeof(float)+sizeof(u16))))
		return false;

	return true;
}


/*-----------------------------------------------------------------------------
	Destructor
-----------------------------------------------------------------------------*/
BigTouch::~BigTouch()
{
	if (m_initialized)
	{
		SAFEFREE(m_pCalData);
		SAFEFREE(m_pImgData);
		SAFEFREE(m_pRAWData);
		SAFEFREE(m_pMin);
		SAFEFREE(m_pMax);
		SAFEFREE(m_pFloatImg);
		SAFEFREE(m_pScaled);
		m_initialized = false;
	}	
}

/*-----------------------------------------------------------------------------
	Create
-----------------------------------------------------------------------------*/
BigTouch::ERR BigTouch::create(char* filename)
{
	ERR err = connect();
	
	if (err != BigTouch::ERR_NO_ERROR)
		return err;
	
	m_pCalData = (u16*) malloc(m_width*m_height*sizeof(u16)); 
	m_pImgData = (u16*) malloc(m_width*m_height*sizeof(u16));
	m_pRAWData = (u16*) malloc(m_width*m_height*sizeof(u16));
	
	memset(m_pCalData,0,m_width*m_height*sizeof(u16));
	memset(m_pImgData,0,m_width*m_height*sizeof(u16));
	memset(m_pRAWData,0,m_width*m_height*sizeof(u16));
	
	if (!writeCalRAM((u8*) m_pCalData))
		return ERR_CALIBRATION;

	m_pScaled = (float*) malloc(m_width*m_height*sizeof(float));
	m_pFloatImg = (float*) malloc(m_width*m_height*sizeof(float));
	m_pMin = (float*) malloc(m_width*m_height*sizeof(float));
	m_pMax = (float*) malloc(m_width*m_height*sizeof(float));

	memset(m_pScaled,0,m_width*m_height*sizeof(float));
	memset(m_pFloatImg,0,m_width*m_height*sizeof(float));
	memset(m_pMin,0,m_width*m_height*sizeof(float));
	memset(m_pMax,0,m_width*m_height*sizeof(float));


	if (strlen(filename))
	{
		if (!loadConfig(filename))
			return ERR_READ_ERROR;
	}
	if (!writeCalRAM((u8*) m_pCalData))		// calibrate at least twice ...
		return ERR_CALIBRATION;

	m_initialized = true;
	
	return ERR_NO_ERROR;
}

/*-----------------------------------------------------------------------------
	Error-String zurückliefern
-----------------------------------------------------------------------------*/
string BigTouch::getErrString(BigTouch::ERR error)
{
	switch (error)
	{
		case ERR_DEVICE_NOT_FOUND:
			return "Device not found!";
		case ERR_CONNECTING:
			return "Couldn't connect!";
		case ERR_ECHO:
			return "No Echo from device!";
		case ERR_CALIBRATION:
			return "Error setting calibration data!";
		case ERR_READ_ERROR:
			return "Error reading config file!";
		case ERR_NO_ERROR:
			return ""; 
	}
	return "Unknown Error!";
}


/*-----------------------------------------------------------------------------
	Send Command Without Data
-----------------------------------------------------------------------------*/
bool BigTouch::SendCmd(u16 cmd)
{
	int length=2;
	int sent = 0;
	sent += m_pUSB->write((char*) &length,2);
	sent += m_pUSB->write((char*) &cmd,length);
	return (sent==4);
}

/*-----------------------------------------------------------------------------
	Send Command With Data
-----------------------------------------------------------------------------*/
bool BigTouch::SendCmdBuf(u16 cmd, u8* buf, int sl)
{
	u16* dummy = (u16*) malloc(sl+sizeof(u16));
	dummy[0]=cmd;
	memcpy((void*) &dummy[1],(void*) buf,sl);

	int length = sl+sizeof(u16);
	u32 sent = 0;
	sent += m_pUSB->write((char*) &length,2);
	sent += m_pUSB->write((char*) dummy,length);
	free(dummy);
	return (sent==length+sizeof(u16));
}

/*-----------------------------------------------------------------------------
	Receive Data
-----------------------------------------------------------------------------*/
bool BigTouch::ReceiveData(u8* buf, int sl)
{
	int length=0;
	u32 r = m_pUSB->read((char*) &length,2);
	if ((r==2) && (length==sl) && (length))
		r = m_pUSB->read((char*) buf,length);
	return (r==(u32) sl);
}

/*-----------------------------------------------------------------------------
	Get Sensor Data
-----------------------------------------------------------------------------*/
bool BigTouch::RequestData(u8* data)
{
	sem_wait(&semUSB);
//	gettimeofday(&tv1, &tz1);
	SendCmd(CMD_RESET_ADP_RD);
	SendCmd(CMD_READ_DUMMY);

	bool ok = true;
	for (int i=0;i<m_numPanels;i++)
	{
		SendCmd(CMD_READ_DATA);
		ok &= ReceiveData((u8*) (data+512*i),512);
		if (!ok)
			break;
	}
/*
	gettimeofday(&tv2, &tz2);
	totalT = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
	sum+=totalT;
	num++;
	printf("%u\n",sum/num);
*/	
	sem_post(&semUSB);
	return ok;
}

/*-----------------------------------------------------------------------------
	Write Calibration Data
-----------------------------------------------------------------------------*/
bool BigTouch::writeCalRAM(/*u16*/ u8* data)
{
	memcpy(m_pCalData,data,m_width*m_height*sizeof(u16));
	sem_wait(&semUSB);
	bool ok = SendCmd(CMD_RESET_ADP_WR);
	for (int i=0;i<m_numPanels;i++)
	{
		ok &= SendCmdBuf(CMD_WRITE_CAL, (u8*) (data+512*i), 512);
	}
	sem_post(&semUSB);
	return ok;
}

/*-----------------------------------------------------------------------------
	Echo-Test
-----------------------------------------------------------------------------*/
bool BigTouch::echoTest()
{
	sem_wait(&semUSB);
	bool echo =false;
	char stest[]={0x80,0x00,0x11,0x22,0x33,0x44,0};
	char buf[16];

	u32 length = 2+strlen(&stest[2])+1;
	u32 s = m_pUSB->write((char*) &length,2);
	s = m_pUSB->write((char*) stest,length);

	s = m_pUSB->read((char*) &length, 2);
	if ((2+strlen(&stest[2])+1)==length)
	{
		s = m_pUSB->read((char*) buf,length);
		if (!strcmp(&stest[2],(char*) &buf[2]))
			echo=true;
	}
	sem_post(&semUSB);
	return echo;
}

/*-----------------------------------------------------------------------------
	an den BigTouch-Sensor connecten
-----------------------------------------------------------------------------*/
BigTouch::ERR BigTouch::connect()
{
	m_pUSB = new CUSBDev(ID_VENDOR, ID_PRODUCT);
	
	if (!m_pUSB)
		return ERR_DEVICE_NOT_FOUND;
		
	if (!m_pUSB->open())
		return ERR_CONNECTING;
	
	if (!echoTest())
		return ERR_ECHO;
	
	return ERR_NO_ERROR;
}

/*-----------------------------------------------------------------------------
	static entry point
-----------------------------------------------------------------------------*/
void* BigTouch::entryPoint(void* pthis)
{
	BigTouch*	pBT = (BigTouch*) pthis;
	pBT->run(0);
	
	return 0;
}

/*-----------------------------------------------------------------------------
	Get RAW Data
-----------------------------------------------------------------------------*/
void BigTouch::getImgData(u16* dst)
{
	sem_wait(&m_sem);
	memcpy(dst,m_pImgData,m_width*m_height*sizeof(u16));
	m_newData = false;
	sem_post(&m_sem);
}

/*-----------------------------------------------------------------------------
	Get Filtered and Scaled data
-----------------------------------------------------------------------------*/
void BigTouch::getFilteredData(float* dst)
{
	sem_wait(&m_sem_filter);
	memcpy(dst,m_pScaled,m_width*m_height*sizeof(float));
	m_newData = false;
	sem_post(&m_sem_filter);
}

/*-----------------------------------------------------------------------------
	Get RAW Data
	Unshuffeld data ... easier to calibrate with!
-----------------------------------------------------------------------------*/
void BigTouch::getRawData(u16* dst)
{
	sem_wait(&m_sem);
	memcpy(dst,m_pRAWData,m_width*m_height*sizeof(u16));
	m_newData = false;
	sem_post(&m_sem);
}

/*-----------------------------------------------------------------------------
	Daten Shufflen
-----------------------------------------------------------------------------*/
void BigTouch::shuffle(u16* dst, u16* src)
{
	const int muxschema[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	const int panelschema[6]={5,3,1,4,2,0};
//	const int rowschema[16]={15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};

	for (int y=0;y<m_height;y++)
	{
		for (int x=0;x<m_width;x++)
		{
			unsigned short* _dst = (unsigned short*) (dst+y*m_width+x);

			int py = y/16;
			int px = x/16;
			int pnr = py*m_numX+px;

			
			unsigned short* _src = (unsigned short*) (src+(((y & 0xf)<<4)+(x & 0xf))*m_numPanels+panelschema[pnr]);
			
			*_dst = *_src;
		}
	}	
}

/*-----------------------------------------------------------------------------
	Daten shufflen und abspeichern
-----------------------------------------------------------------------------*/
void BigTouch::processData()
{
	sem_wait(&m_sem);
	shuffle(m_pImgData,m_pRAWData);
	m_newData = true; 
	sem_post(&m_sem);
}

/*-----------------------------------------------------------------------------
	Gauss-Convolution with seperated kernel
	Border will be zeroed
-----------------------------------------------------------------------------*/
void BigTouch::convolute(float* img, int num)
{
	float g[m_height][m_width];
	float tmp[m_height][m_width];
	
	memcpy(tmp,img,sizeof(tmp));
	
	for (int i=0;i<num;i++)
	{	
		for (int y=1;y<m_height-1;y++)
		{
			for (int x=1;x<m_width-1;x++)
			{
				g[y][x]=1.0*tmp[y][x-1]+2.0*tmp[y][x]+1.0*tmp[y][x+1];	
			}
		}
	
		for (int x=0;x<m_width;x++)
		{
			for (int y=0;y<m_height;y++)
			{
				if (!x || !y || (x==(m_width-1)) || (y==(m_height-1)))
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
	running thread
-----------------------------------------------------------------------------*/
int BigTouch::run(void* arg)
{
	while (m_cont)
	{
//		cout<<"thread\n";
		sem_wait(&m_sem);
		bool ok = RequestData((u8*) m_pRAWData);
		sem_post(&m_sem);
		if (ok)
		{
			processData();

// filter data and scale
			sem_wait(&m_sem_filter);
			for (int i=0;i<m_width*m_height;i++) 
			{
				m_pFloatImg[i] = /*0.20*m_pFloatImg[i] + 0.80**/((float) m_pImgData[i] / 65535.0);
				m_pScaled[i] = (m_pFloatImg[i] - m_pMin[i])/(m_pMax[i]-m_pMin[i]+(1.0/65535.0));
			}				
// convolute once
			convolute(m_pScaled,1);
			sem_post(&m_sem_filter);
		}		
		usleep(10000);	// 10ms
		
	}
	return 0;
}

/*-----------------------------------------------------------------------------
	start thread
-----------------------------------------------------------------------------*/
bool BigTouch::startThread()
{
	if (!m_initialized)
		return false;
	
	sem_init(&m_sem, 0, 1);
	sem_init(&m_sem_filter, 0, 1);
	setCont(true);
	pthread_create(&m_thread, NULL, BigTouch::entryPoint,  (void*) this);
	
	return true;
}

/*-----------------------------------------------------------------------------
	stop thread
-----------------------------------------------------------------------------*/
bool BigTouch::stopThread()
{	
	if (!m_initialized)
		return false;
	
	setCont(false);
	pthread_join(m_thread, NULL);
	
	return true;
}


