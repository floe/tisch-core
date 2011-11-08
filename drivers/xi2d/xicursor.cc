/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

// based on some code ripped from libXcursor (MIT license).
// TODO: is LGPL relicensing okay here?

#include <X11/extensions/XInput2.h>
#include <X11/extensions/Xrender.h>
#include "PicoPNG.h"
#include <iostream>


Display* dpy;
int screen;

static int nativeByteOrder() {
	int x = 1;
	return (*((char *) &x) == 1) ? LSBFirst : MSBFirst;
}


Cursor load( const char* file ) {

	PNGImage image( file );
	Cursor cursor = None;
    
	XImage  ximage;
	Pixmap  pixmap;
	Picture picture;
	GC      gc;
	XRenderPictFormat* format;

	ximage.width = image.width();
	ximage.height = image.height();
	ximage.xoffset = 0;
	ximage.format = ZPixmap;
	ximage.data = (char*)image.data();
	ximage.byte_order = nativeByteOrder();
	ximage.bitmap_unit = 32;
	ximage.bitmap_bit_order = ximage.byte_order;
	ximage.bitmap_pad = 32;
	ximage.depth = 32;
	ximage.bits_per_pixel = 32;
	ximage.bytes_per_line = image.width() * 4;
	ximage.red_mask = 0xff0000;
	ximage.green_mask = 0x00ff00;
	ximage.blue_mask = 0x0000ff;
	ximage.obdata = NULL;

	if (!XInitImage (&ximage)) return None;

	pixmap = XCreatePixmap( dpy, RootWindow (dpy, screen), image.width(), image.height(), 32 );
	//printf("%x\n", pixmap );

	gc = XCreateGC( dpy, pixmap, 0, NULL );
	//printf("%x\n", gc );
	XPutImage( dpy, pixmap, gc, &ximage, 0, 0, 0, 0, image.width(), image.height() );
	XFreeGC( dpy, gc );

	format = XRenderFindStandardFormat( dpy, PictStandardARGB32 );
	picture = XRenderCreatePicture( dpy, pixmap, format, 0, NULL );
	XFreePixmap( dpy, pixmap );

	cursor = XRenderCreateCursor( dpy, picture, 0, 0 ); //image->xhot, image->yhot);
	XRenderFreePicture( dpy, picture );

	return cursor;
}


int main( int argc, char* argv[] ) {

	if (argc != 3) { printf("usage: xicursor ptrid /path/to/cursor.png\n"); return 1; }

	dpy = XOpenDisplay( NULL );
	screen = DefaultScreen(dpy);

	Window win = RootWindow(dpy,screen);

	int id = atoi(argv[1]);
	Cursor foo = load(argv[2]);

	Status res = XIDefineCursor( dpy, id, win, foo );

	XCloseDisplay(dpy);

	return res;
}

