#ifndef __BIGTOUCH__
#define __BIGTOUCH__

#include <string>
#include "cusbdev.h"
#include <pthread.h> 
#include <semaphore.h> 

using namespace std;

// identifies the USB device
#define ID_VENDOR	0x03eb
#define ID_PRODUCT	0x6125

#define CMD_RESET_ADP_RD	0x0010
#define CMD_READ_DATA		0x0011
#define CMD_READ_DUMMY		0x0012
#define CMD_RESET_ADP_WR	0x0020
#define CMD_WRITE_CAL		0x0021
#define CMD_SET_INTTIME		0x0040
#define CMD_ECHO			0x0080

class BigTouch {
	public:
		enum ERR { ERR_NO_ERROR=0, ERR_DEVICE_NOT_FOUND=-1, ERR_CONNECTING=-2, ERR_ECHO=-3,
				   ERR_CALIBRATION=-4, ERR_READ_ERROR=-5 };

		BigTouch();
		virtual ~BigTouch();
		
		BigTouch::ERR create(char* filename="");
		BigTouch::ERR connect();

		string getErrString(BigTouch::ERR error);

		bool writeCalRAM(u8* data);
		u16* getCalData() { return m_pCalData; }

		
// physical size ... should be 48*32px
		int	getWidth() { return m_width; }
		int getHeight() { return m_height; }
		int getNumPanels() { return m_numPanels; }
		
		bool startThread();
		bool stopThread();
		
		void getImgData(u16* dst);
		void getRawData(u16* dst);
		void getFilteredData(float* dst);
		
		void shuffle(u16* dst, u16* src);
		
		
		bool newData() { return m_newData; }
		
	protected:
		static void* entryPoint(void* pthis);
		int run(void* arg);

		bool loadConfig(char* filename);

		void processData();
		void convolute(float* img, int num);

				
		bool SendCmd(u16 cmd);
		bool SendCmdBuf(u16 cmd, u8* buf, int sl);
		bool ReceiveData(u8* buf, int sl);
		bool RequestData(u8* data);
		
		bool echoTest();
		
		void setCont(bool state) { m_cont = state; }
		
	protected:
		CUSBDev*		m_pUSB;	
		int				m_numPanels;
		int 			m_numX;
		int 			m_numY;
		int				m_width;
		int				m_height;
		volatile bool	m_cont;
		pthread_t		m_thread;
		u16*			m_pCalData;
		u16*			m_pImgData;
		u16*			m_pRAWData;
		bool 			m_initialized;
		volatile bool	m_newData;
		
		int 			m_cMux[16];
		int 			m_cPanel[6];
		int 			m_cRow[16];		
		
		float*			m_pMin;
		float*			m_pMax;		
		float*			m_pFloatImg;
		float*			m_pScaled;
};

#endif
