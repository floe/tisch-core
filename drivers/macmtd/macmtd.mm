/**
 * macmtdd
 * 
 * connects mac multitouch devices (such as touchpads, the magic mouse or the magic trackpad)
 * to libtisch
 *
 * (c) 2011 Martin Kleinhans <mail@mkleinhans.de>
 */

#include <math.h>
#include <unistd.h>
#include <CoreFoundation/CoreFoundation.h>

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#include "output.h"

//========================================================================
// globals
//========================================================================
BOOL g_bVerbose = false;
float g_fArea 	= 300.0f;
float g_fWidth 	= 1024.0f, g_fHeight = 768.0f;
int g_iMode 	= 1;

//========================================================================
// interface with private MultitouchSupport.framework
//========================================================================

typedef struct { float x,y; } mtPoint;
typedef struct { mtPoint pos,vel; } mtReadout;

typedef struct {
	int frame;
	double timestamp;
	int identifier, state, foo3, foo4;
	mtReadout normalized;
	float size;
	int zero1;
	float angle, majorAxis, minorAxis; // ellipsoid
	mtReadout mm;
	int zero2[2];
	float unk2;
} Finger;

extern "C" {
	typedef void *MTDeviceRef;
	typedef int (*MTContactCallbackFunction)(int,Finger*,int,double,int);

	MTDeviceRef MTDeviceCreateDefault();
	void MTRegisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
	void MTDeviceStart(MTDeviceRef, int); // thanks comex
	CFMutableArrayRef MTDeviceCreateList(void);
}

//========================================================================
// frame callback
//========================================================================
int callback(int device, Finger *data, int nFingers, double timestamp, int frame) {
	if(g_iMode == 1 && nFingers < 2) {
		return 0;
	}

	if(g_bVerbose) {
		printf("Device: %d\n",device);
	}
	
	NSPoint mouseLoc;
	Finger *f;
	int i;
	float x,y;

	mouseLoc = [NSEvent mouseLocation];

	out_startFrame();

	for (i=0; i<nFingers; i++) {
		f = &data[i];
		
		if(g_bVerbose) {
			printf("Frame %7d: Angle %6.2f, ellipse %6.3f x%6.3f; "
			   "position (%6.3f,%6.3f) vel (%6.3f,%6.3f) "
			   "ID %d, state %d [%d %d?] size %6.3f, %6.3f?\n",
			   f->frame,
			   f->angle * 90 / atan2(1,0),
			   f->majorAxis,
			   f->minorAxis,
			   f->normalized.pos.x,
			   f->normalized.pos.y,
			   f->normalized.vel.x,
			   f->normalized.vel.y,
			   f->identifier, f->state, f->foo3, f->foo4,
			   f->size, f->unk2);
		}
		
		if(g_iMode == 1) {	
			x = ((f->normalized.pos.x-0.5f)*(g_fArea/g_fWidth) + mouseLoc.x/g_fWidth);
			y = ((f->normalized.pos.y-0.5f)*(g_fArea/g_fHeight) + mouseLoc.y/g_fHeight);
		} else {
			x = f->normalized.pos.x;
			y = f->normalized.pos.y;
		}

		out_blob(f->identifier, f->state == 7 ? 0 : 1, x, y);
	}
	
	if(g_bVerbose) {
		printf("\n");
	}
	
	out_endFrame();

	return 0;
}

int main(int argc, char* argv[]) {
	printf("macmtd - libTISCH 2.0 Mac MultiTouch adapter\n");
	printf("(c) 2011 by Martin Kleinhans <mail@mkleinhans.de>\n");
	
	out_init("127.0.0.1");
	
	int opt,i,deviceIndex = -1;
	for (opt = 1; opt < argc; opt++ ) {
		
		// device index - if not specified, all devices are used simultaneously
		if (strcmp(argv[opt],"-d") == 0 && opt < argc-1) {
			deviceIndex = atoi(argv[++opt]);
		}

		// screen width
		if (strcmp(argv[opt],"-w") == 0 && opt < argc-1) {
			g_fWidth = atoi(argv[++opt]);
		}

		// screen height
		if (strcmp(argv[opt],"-h") == 0 && opt < argc-1) {
			g_fHeight = atoi(argv[++opt]);
		}

		// coordinate mode: 0 = absolute, 1 = relative to mouse cursor
		if (strcmp(argv[opt],"-m") == 0 && opt < argc-1) {
			g_iMode = atoi(argv[++opt]);
		}

		// movement area for relative mode
		if (strcmp(argv[opt],"-a") == 0 && opt < argc-1) {
			g_fArea = atoi(argv[++opt]);
		}
		
		// verbose mode
		if (strcmp(argv[opt],"-v") == 0) {
			g_bVerbose = true;
			printf("verbode mode active\n");
		}
		
		// just list devices and quit
		if(strcmp(argv[opt],"-l") == 0) {
			NSMutableArray* deviceList = (NSMutableArray*)MTDeviceCreateList();
			printf("Found %d devices:\n",(int)[deviceList count]);
			for(i = 0; i<[deviceList count]; i++) {
				printf("- Device %d\n",i);	
			}
			printf("\n");

			return 0;
		}
	}

	NSMutableArray* deviceList = (NSMutableArray*)MTDeviceCreateList();
	if(deviceIndex >= 0) {
		if(deviceIndex < [deviceList count]) {
			MTRegisterContactFrameCallback([deviceList objectAtIndex:deviceIndex], callback);
			MTDeviceStart([deviceList objectAtIndex:deviceIndex], 0);
		} else {
			printf("ERROR: device index beyond bounds!");
		}
	} else {
		for(i = 0; i<[deviceList count]; i++) {
			MTRegisterContactFrameCallback([deviceList objectAtIndex:i], callback);
			MTDeviceStart([deviceList objectAtIndex:i], 0);
		}
	}

	printf("Ctrl-C to abort\n");
	sleep(-1);
	
	out_deinit();

	return 0;
}
