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
#include <usb.h>
#include "cusbdev.h"
#include "global.h"

CUSBDev::CUSBDev(u16 vendor, u16 product) : m_device(0), m_vendor(vendor), m_product(product), m_opened(0),
	 m_timeout(1000)
{
	usb_init();
}


CUSBDev::~CUSBDev()
{
}


struct usb_device* CUSBDev::find_usbdevice(int vendorid, int productid)
{
	usb_bus *busses;
	usb_bus *bus;
	struct usb_device *dev;

    // Refresh libusb structures
	usb_find_busses();
	usb_find_devices();
	busses = usb_get_busses();
    // find device
	for(bus=busses ; bus ; bus=bus->next)
	{
		for(dev=bus->devices ; dev ; dev=dev->next)
		{
			if(dev->descriptor.idVendor == vendorid && dev->descriptor.idProduct == productid)
				return dev;
		}
	}
	return NULL;
}

bool CUSBDev::reset()
{
	if (!m_opened)
		return false;
/*	struct usb_device *dev;
	dev = find_usbdevice(m_vendor, m_product);

	if (!dev)
		return false;
*/	
	usb_reset(m_device);
	
	return true;
}
	

bool CUSBDev::open()
{
	struct usb_device *dev;
	
	dev = find_usbdevice(m_vendor, m_product);
	
	if (!dev)
		return false;
	
	m_device = usb_open(dev);
		
// usbfs complains about not having claimed interface 1 although it is not used ...
// this solves the message but don't know why
	if (usb_claim_interface(m_device, 1)<0)
	{
		usb_close(m_device);
		return false;
	}
// this is important ... 
	if (usb_claim_interface(m_device, 0)<0)
	{
		usb_close(m_device);
		return false;
	}
	if (usb_set_altinterface(m_device, 0)<0)
	{
		usb_close(m_device);
		return false;
	}
	
	m_opened = true;
	return true;
}

bool CUSBDev::close()
{
	bool res = true;
	
//	reset();
	
	if (usb_release_interface(m_device,0)<0)
		res = false;
	
	if (usb_release_interface(m_device,1)<0)
		res = false;
	
	usb_close(m_device);
	
	m_opened=false;
	
	return res;
}
	
int CUSBDev::write(char *buff, int len)
{
	if (!m_opened)
		return -1;
	
	return usb_bulk_write(m_device, 0x1, buff, len, m_timeout);
}

int CUSBDev::read(char *buff, int len) 
{
	if (!m_opened)
		return -1;
	
	return usb_bulk_read(m_device, 0x82, buff, len, m_timeout);
}	
