/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _GLUTWINDOW_H_
#define _GLUTWINDOW_H_

#include <string>

#include <Vector.h>

#include "RGBImage.h"
#include "ShortImage.h"
#include "IntensityImage.h"

#include "GLUTWrapper.h"


class TISCH_SHARED GLUTWindow {

	public:

		GLUTWindow( int w, int h, const std::string& title, int mode = GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL );
		virtual ~GLUTWindow();

		void show( const RGBImage&       img, int x, int y ) const;
		void show( const ShortImage&     img, int x, int y ) const;
		void show( const IntensityImage& img, int x, int y ) const;

		void print( const std::string& text, int x, int y ) const;
		void drawRectangleBackground( int x, int y, int size_x, int size_y, int border) const;
		void drawPolygon( std::vector<Point*> points, bool invert = 0, int height = 0) const;
		void title( const std::string& text ) const;

		void clear( float red = 0.0, float green = 0.0, float blue = 0.0, float alpha = 0.0 );

		void mode2D();
		void swap();
		void run();

		int getWidth ();
		int getHeight();

		virtual void idle();
		virtual void display();

		virtual void reshape( int w, int h );

		virtual void keyboard( int key, int x, int y );
		virtual void mouse( int num, int button, int state, int x, int y );
		virtual void passive( int num, int x, int y );
		virtual void motion( int num, int x, int y );
		virtual void entry( int num, int state );

	protected:

		int width;
		int height;

		int win;

};

#endif // _GLUTWINDOW_H_

