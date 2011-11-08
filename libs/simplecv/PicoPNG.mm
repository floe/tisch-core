/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "PicoPNG.h"
#include <stdexcept>
#include <QuartzCore/QuartzCore.h>
#include <UIKit/UIImage.h>


PNGImage::PNGImage( const std::string& file ) {

	NSString* filename = [[NSString alloc] initWithCString:(file.c_str())];
	CGImageRef image = [UIImage imageNamed:filename].CGImage;
	[filename release];

	if (image) {

		m_width  = CGImageGetWidth(  image );
		m_height = CGImageGetHeight( image );

		buffer.reserve( m_height * m_height * 4 );

		// create a colorspace & drawing context
		CGColorSpaceRef colorspace = CGColorSpaceCreateWithName( kCGColorSpaceGenericRGB );

		// the sensible solution /without/ premultiplied alpha does not work, so
		// these images would need glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
		CGContextRef context = CGBitmapContextCreate( &(buffer[0]), m_width, m_height, 8,
			m_width*4, colorspace, kCGImageAlphaPremultipliedLast );
	
		// draw image into context and release everything	
		CGContextDrawImage( context, CGRectMake( 0.0, 0.0, float(m_width), float(m_height) ), image );
		CGContextRelease( context );
		CGColorSpaceRelease( colorspace );
	
	} else throw std::runtime_error( std::string( "Could not open " ) + file );
}


unsigned char* PNGImage::data() { return &(buffer[0]); }
unsigned long int PNGImage::width() { return m_width; }
unsigned long int PNGImage::height() { return m_height; }

