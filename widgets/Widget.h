/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include <vector>
#include <set>

#include <GLUTWrapper.h>
#include <Texture.h>
#include <Matcher.h>
#include <Region.h>


// class forward declarations
class Widget;
class Container;

// projection matrix and viewport, used for 
// back-projection to screen coordinates
extern TISCH_SHARED GLdouble g_proj[16];
extern TISCH_SHARED GLint    g_view[4];

// list of all currently existing widgets, used
// for sanity checking of incoming identifiers
extern TISCH_SHARED std::set<Widget*> g_widgets;

// the global gesture matcher object
extern TISCH_SHARED Matcher* g_matcher;


// call inside constructor if a default texture is desired
// variant: memfun. template <class W> void getDefaultTex( const char* name ) { .. }
#define getDefaultTex(usertex,name) { \
	if (!usertex) try { \
		static RGBATexture* tex = new RGBATexture( TISCH_PREFIX name ); \
		mytex = tex; \
	} catch(...) { } \
}


// the (partially) abstract Widget base class
class TISCH_SHARED Widget {

	friend class Container;

	public:

		Widget( int _w, int _h, int _x = 0, int _y = 0, double _angle = 0.0, RGBATexture* _tex = 0, unsigned int _regflags = INPUT_TYPE_ANY ):
			region(_regflags), shadow(false), hide(false),
			w(_w), h(_h), x(_x), y(_y), sx(1.0), sy(1.0), angle(_angle),
			asx(1.0), asy(1.0), absangle(0.0), abspos(0,0,0),
			mytex(_tex), parent(0)
		{
			if (angle > 2*M_PI) angle *= M_PI/180.0;
			g_widgets.insert( this );

			mycolor[0] = 0.0; 
			mycolor[1] = 0.0; 
			mycolor[2] = 0.0; 
			mycolor[3] = 1.0;
		}

		virtual ~Widget() {
			g_widgets.erase( this );
			unregister();
		}


		Vector glOutline2d( double ox, double oy, int push = 1 ) {
			GLdouble resx, resy, resz;
			gluProject( ox, oy, 0.0, m_model, g_proj, g_view, &resx, &resy, &resz );
			Vector res( resx/g_view[2], resy/g_view[3], resz );
			if (push) region.push_back( res );
			return res;
		}

		virtual void outline() {
			glOutline2d( -w/2.0, -h/2.0 );
			glOutline2d(  w/2.0, -h/2.0 );
			glOutline2d(  w/2.0,  h/2.0 );
			glOutline2d( -w/2.0,  h/2.0 );
		}

		virtual void update( Widget* target = 0 );

		virtual void doUpdate( Widget* target = 0 ) {
			
			bool do_register = true;
			if ((target) && (target != this)) do_register = false;

			// retrieve the local modelview matrix
			enter(); glGetDoublev( GL_MODELVIEW_MATRIX, m_model ); leave();

			// project two unit vectors to determine absolute rotation & scale
			Vector v1 = glOutline2d( 0.0, 0.0, 0 );
			Vector v2 = glOutline2d( 1.0, 0.0, 0 );
			Vector v4 = glOutline2d( 0.0, 1.0, 0 );

			v2 = v2 - v1; asx = v2.length();
			v4 = v4 - v1; asy = v4.length();

			abspos = v1;
			v1.set(1,0,0); v2.z = 0;
			v2.normalize();
			absangle = acos(v1*v2);
			if (v2.y < 0) absangle = 2*M_PI - absangle;

			if (do_register) {
				region.clear();
				if (!hide) outline();
				g_matcher->update( (unsigned long long)this, &region );
			}
		}

		virtual void raise( Widget* widget = 0 );

		virtual void lower( Widget* widget = 0 );

		void transform( Vector& vec, int abs = 0, int local = 0 );

		void texture( RGBATexture* tex ) { mytex = tex; }
		void color( double r, double g, double b, double a ) { mycolor[0] = r; mycolor[1] = g; mycolor[2] = b; mycolor[3] = a; }
		void setangle( double _angle ) { angle = _angle; if (fabs(angle) > 2*M_PI) angle *= M_PI/180.0; }

		virtual void draw() {
			enter();
			paint();
			leave();
		}

		virtual void action( Gesture* gesture ) = 0;

		void paint_tex( RGBATexture* tex = 0 ) {

			double tw = 1.0; 
			double th = 1.0;

			if (tex) {
				tex->bind();
				tw = tex->width();
				th = tex->height();
			}

			glColor4dv( mycolor );

			GLvector texcoord[4];
			GLvector vertices[4];

			texcoord[0] = GLvector( 0.0,  th ); vertices[0] = GLvector( -w/2.0, -h/2.0, 0.0 );
			texcoord[1] = GLvector(  tw,  th ); vertices[1] = GLvector(  w/2.0, -h/2.0, 0.0 );
			texcoord[2] = GLvector( 0.0, 0.0 ); vertices[2] = GLvector( -w/2.0,  h/2.0, 0.0 );
			texcoord[3] = GLvector(  tw, 0.0 ); vertices[3] = GLvector(  w/2.0,  h/2.0, 0.0 );

			glutPaintArrays( 4, vertices, texcoord );

			if (tex) tex->release();
		}

		virtual void enter( double z = 0.0 ) {
			glPushMatrix();
			glTranslated( x, y, z );
			glScaled( sx, sy, 1 );
			glRotated( 360*angle/(2*M_PI), 0, 0, 1 );
		}

		virtual void paint( bool update_stencil = false ) {

			if (update_stencil) glEnable( GL_STENCIL_TEST );
			paint_tex( mytex );
			if (update_stencil) glDisable( GL_STENCIL_TEST );

			if (shadow) try {

				static RGBATexture* shadowtex = new RGBATexture( TISCH_PREFIX "Shadow.png" );

				glPushMatrix();

				glTranslatef( 0, 0, -0.05 );
				glScalef( 1.075, 1.075, 1.0 );

				paint_tex( shadowtex );

				glPopMatrix();

			} catch (...) { shadow = false; }
		}

		void leave() {
			glPopMatrix();
		}

		Region region;
		bool shadow;
		bool hide;

		double w,h,x,y;          // start size & position
		double sx,sy,angle;      // relative scale & rotation

		double asx,asy,absangle; // absolute scale & rotation
		Vector abspos;           // absolute position

	protected:

		void unregister() {
			g_matcher->remove( (unsigned long long)this );
		}

		RGBATexture* mytex;
		GLdouble mycolor[4];

		Container* parent;

		GLdouble m_model[16];    // modelview matrix
};

#endif // _WIDGET_H_

