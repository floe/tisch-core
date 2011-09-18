/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

// based on test_xi2.c from xinput by Peter Hutterer

#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <BasicBlob.h>
#include <Socket.h>
#include <Thread.h>
#include <tisch.h>
#include <map>


UDPSocket output( INADDR_ANY, 0 );

std::map<int,BasicBlob> blobs;
int framenum = 0;


struct OutputThread: public Thread {

	OutputThread(): Thread() { }

	void* run() {

		while (1) {

			lock();
			
			// send everything out
			for (std::map<int,BasicBlob>::iterator blob = blobs.begin(); blob != blobs.end(); blob++) {

				blob->second.peak = blob->second.pos;

				if (blob->second.value >= 1) {
					blob->second.id  = blob->first+1;
					blob->second.pid = 0;
					output << "shadow " << blob->second << std::endl;
				}

				if (blob->second.value >= 2) {
					blob->second.id  = blob->first;
					blob->second.pid = blob->first+1;
					output << "finger " << blob->second << std::endl;
				}

			}

			release();

			output << "frame " << framenum++ << std::endl;
		
			usleep( 16666 ); // 60 Hz
		}

		return 0;
	}

};

OutputThread outthr;


int main( int argc, char* argv[] ) {

	std::cout << "xi2d - libTISCH 2.0 XInput2 adapter" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	Display* display = XOpenDisplay( NULL );

	// check for XInput presence & opcode
	int xi_opcode, event, error;
	if (!XQueryExtension( display, "XInputExtension", &xi_opcode, &event, &error )) { 
		printf("XInput extension not available.\n");
		return 1;
	}

	// create a simple window
	Window win = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 200, 200, 0, 0, WhitePixel(display, 0));
	XStoreName( display, win, "xi2d helper window" );
	XMapWindow( display, win );
	XSync( display, False );

	// select motion events for entire screen
	XIEventMask mask;
	mask.deviceid = XIAllDevices;
	mask.mask_len = XIMaskLen( XI_LASTEVENT );
	mask.mask = (unsigned char*)calloc( mask.mask_len, sizeof(char) );

	XISetMask( mask.mask, XI_Motion );
	XISetMask( mask.mask, XI_ButtonPress );
	XISetMask( mask.mask, XI_ButtonRelease );

	XISelectEvents( display, DefaultRootWindow(display), &mask, 1 );
	free( mask.mask );

	// set UDP target and start UDP thread
	output.target( INADDR_LOOPBACK, TISCH_PORT_CALIB );
	outthr.start();

	while(1) {

		XEvent ev;
		XGenericEventCookie* cookie = (XGenericEventCookie*)&ev.xcookie;
		XNextEvent( display, (XEvent*)&ev );

		if (XGetEventData(display, cookie) && cookie->type == GenericEvent && cookie->extension == xi_opcode) {

			XIDeviceEvent* xide = (XIDeviceEvent*)cookie->data;

			outthr.lock();
			int button = 0;

			switch (cookie->evtype) {

				case XI_ButtonPress: // button number is xide->detail
					button = xide->detail;
					if (button == 4 || button == 5) {
						double angle = 2.0/180*M_PI;
						if (button == 4) angle = -angle;
						blobs[xide->deviceid].axis1.rotate( angle );
						blobs[xide->deviceid].axis2.rotate( angle );
						break;
					}
					blobs[xide->deviceid].value = 2;
					break;
				case XI_ButtonRelease:
					blobs[xide->deviceid].value = 1;
					break;
				case XI_Motion:
					blobs[xide->deviceid].pos.x = xide->root_x;
					blobs[xide->deviceid].pos.y = 1050-xide->root_y;
					break;
				default:
					std::cout << "Unknown event type " << cookie->evtype << " received." << std::endl;
					break;
			}

			outthr.release();
		}

		XFreeEventData(display, cookie);
	}

	XDestroyWindow(display, win);

	return 0;
}

