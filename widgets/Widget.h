/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include <vector>
#include <set>

#include <GLUTWrapper.h>
#include <Texture.h>
#include <Region.h>


// projection matrix and viewport, used for 
// back-projection to screen coordinates
extern TISCH_SHARED GLdouble g_proj[16];
extern TISCH_SHARED GLint    g_view[4];

// list of all currently existing widgets, used
// for sanity checking of incoming identifiers
class Widget;
extern TISCH_SHARED std::set<Widget*> g_widgets;


// call inside constructor if a default texture is desired
// variant: memfun. template <class W> void getDefaultTex( const char* name ) { .. }
#define getDefaultTex(usertex,name) { \
	if (!usertex) try { \
		static RGBATexture* tex = new RGBATexture(name); \
		mytex = tex; \
	} catch(...) { } \
}


// the (partially) abstract Widget base class
class TISCH_SHARED Widget {

	friend class Container;

	public:

		Widget( int _w, int _h, int _x = 0, int _y = 0, double _angle = 0.0, RGBATexture* _tex = 0, int _regflags = (1<<INPUT_TYPE_COUNT)-1 ):
			mytex(_tex), shadow(false), region(_regflags), parent(0), regstream(0), 
			w(_w), h(_h), x(_x), y(_y), sx(1.0), sy(1.0), angle(_angle),
			asx(1.0), asy(1.0), absangle(0.0), abspos(0,0,0)
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
			*regstream << "region " << (unsigned long long)this << " 0 0 0" << std::endl;
		}


		Vector glOutline2d( double ox, double oy, int push = 1 ) {
			GLdouble resx, resy, resz;
			gluProject( ox, oy, 0.0, m_model, g_proj, g_view, &resx, &resy, &resz );
			Vector res(resx,resy,resz);
			if (push) region.push_back( res );
			return res;
		}

		virtual void outline() {
			glOutline2d( -w/2.0, -h/2.0 );
			glOutline2d(  w/2.0, -h/2.0 );
			glOutline2d(  w/2.0,  h/2.0 );
			glOutline2d( -w/2.0,  h/2.0 );
		}

		virtual void update( Widget* target = 0 ) {
			if( this->parent == 0 )
				//Problem: hier wird doUpdate von Container, anstatt von MasterContainer aufgerufen.
				doUpdate( target );
			else
				(this->parent)->update( target );
		}

		virtual void doUpdate( Widget* target = 0, std::ostream* ost = 0 ) {
			
			bool do_register = true;
			if ((target) && (target != this)) do_register = false;

			if (ost) regstream = ost;
			if (do_register && (!regstream)) return;

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
				outline();
				*regstream << "region " << (unsigned long long)this << " " << region << std::endl;
			}
		}

		virtual void raise( Widget* widget = 0 ) { if (parent) parent->raise( this ); }

		void transform( Vector& vec, int abs = 0, int local = 0 ) { 
 			if (abs) vec = vec - abspos; 
			if (parent && !local) {
				vec.x /= parent->asx;
				vec.y /= parent->asy;
				vec.rotate( parent->absangle );
			} else {
				vec.x /= asx;
				vec.y /= asy;
				vec.rotate( absangle );
			}
		}

		void texture( RGBATexture* tex ) { mytex = tex; }
		void color( double r, double g, double b, double a ) { mycolor[0] = r; mycolor[1] = g; mycolor[2] = b; mycolor[3] = a; }

		virtual void draw() {
			enter();
			paint();
			leave();
		}

		virtual void action( Gesture* gesture ) = 0;

	protected:

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

		void enter( double z = 0.0 ) {
			glPushMatrix();
			// TODO: swap Scale/Translate?
			glTranslated( x, y, z );
			glScaled( sx, sy, 1 );
			glRotated( 360*angle/(2*M_PI), 0, 0, 1 );
		}

		void paint() {

			paint_tex( mytex );

			if (shadow) try {

				static RGBATexture* shadowtex = new RGBATexture( "Shadow.png" );

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

		RGBATexture* mytex;
		GLdouble mycolor[4];
		bool shadow;

		Region region;
		Widget* parent;
		std::ostream* regstream;

		GLdouble m_model[16];    // modelview matrix

		double w,h,x,y;          // start size & position
		double sx,sy,angle;      // relative scale & rotation
		double asx,asy,absangle; // absolute scale & rotation
		Vector abspos;           // absolute position
};

#endif // _WIDGET_H_

