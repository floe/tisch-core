/*
 *	SAM7FLASH V1.x.x
 *	written 2006 by Thomas Pototschnig (http://www.oxed.de)
 *
 * The software is delivered "AS IS" without warranty or condition of any    
 * kind, either express, implied or statutory. This includes without	     
 * limitation any warranty or condition with respect to merchantability or   
 * fitness for any particular purpose, or against the infringements of	     
 * intellectual property rights of others.
 *
 * Please don't remove the header!
 *
 * Please report any bug/fix, modification, suggestion to
 *		thomas dot pototschnig at gmx.de
 */
#ifndef CUSBDEV_H
#define CUSBDEV_H

#include "global.h"
#include <usb.h>

/**
	@author Thomas Pototschnig,,, <thomas@graphiti>
*/
class CUSBDev{
public:
    CUSBDev(u16 vendor, u16 product);
    ~CUSBDev();
	
	bool open();
	bool close();
	bool reset();
	
	int read(char* buf, int length);
	int write(char* buf, int length);
	bool isOpened() { return m_opened; }
	
protected:
	struct usb_device* find_usbdevice(int vendorid, int productid);
		
	
protected:
	u16		m_vendor;
	u16		m_product;
	bool	m_opened;
	int		m_timeout;
	
	usb_dev_handle *m_device;
};

#endif
