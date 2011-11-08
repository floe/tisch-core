/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "GLUTWrapper.h"

#include "IntensityImage.h"
#include "RGBImage.h"
#include "ShortImage.h"
#include "PicoPNG.h"

#include <math.h>
#include <Vector.h>

typedef _Vector<GLdouble> GLvector;


#ifndef SWIG
	#if !GL_TEXTURE_RECTANGLE_ARB || TISCH_TEXTURE_LEGACY
		// #warning Using GL_TEXTURE_2D as default target. 
		#define DEFAULT_TEXTURE_TARGET GL_TEXTURE_2D
		#define GL_TEXTURE_RECTANGLE_ARB 0x84F5
	#else
		#define DEFAULT_TEXTURE_TARGET GL_TEXTURE_RECTANGLE_ARB
	#endif
#endif


template < int TEXTURE_TARGET, int TEXTURE_FORMAT, int DATA_FORMAT, int DATA_TYPE > class Texture {

  public:

		Texture( GLint w, GLint h, GLenum filter = GL_LINEAR, GLenum mode = GL_REPLACE ) {
			init( w, h, filter, mode );
		}

		Texture( const char* pngfile, GLenum filter = GL_LINEAR, GLenum mode = GL_REPLACE ) {
			PNGImage* tmp = new PNGImage( pngfile );
			init( tmp->width(), tmp->height(), filter, mode );
			load( tmp->data(), GL_RGBA, GL_UNSIGNED_BYTE );
			delete tmp;
		}

		~Texture() {
			release();
			if (glIsTexture(m_myname)) glDeleteTextures( 1, &m_myname );
		}


		void render( GLint texels = 0, GLenum where = GL_TEXTURE0, GLfloat z = -0.5 ) {

			double ty,qy;
			if (texels == 0) texels = m_width*m_height;

			glPushAttrib( GL_ENABLE_BIT | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			glMatrixMode( GL_PROJECTION ); glPushMatrix(); glLoadIdentity();
			glMatrixMode( GL_MODELVIEW  ); glPushMatrix(); glLoadIdentity();

			// switch off some state
			glDisable( GL_POLYGON_OFFSET_FILL );
			glDisable( GL_CULL_FACE );
			glDisable( GL_LIGHTING );

			GLvector texcoord[4];
			GLvector vertices[4];

			switch (TEXTURE_TARGET) {

				case GL_TEXTURE_RECTANGLE_ARB:

					ty = ceil((double)texels/(double)m_width);
					qy = (2.0 * (ty/(double)m_height)) - 1.0;

					texcoord[0] = GLvector(         0.5, ty-0.5 ); vertices[0] = GLvector( -1.0, -1.0, z );
					texcoord[1] = GLvector( m_width-0.5, ty-0.5 ); vertices[1] = GLvector(  1.0, -1.0, z );
					texcoord[2] = GLvector(         0.5,    0.5 ); vertices[2] = GLvector( -1.0,   qy, z );
					texcoord[3] = GLvector( m_width-0.5,    0.5 ); vertices[3] = GLvector(  1.0,   qy, z );

					break;

				case GL_TEXTURE_2D:

					ty = (double)texels/(m_width*m_height) + (1.0/m_width); //  1.0 / 0.5 / 0.25 ..
					qy = (-2.0 * ty) + 1.0;                                 // -1.0 / 0.0 / 0.50 ..
					ty = 1.0;
					qy = 1.0;

					texcoord[0] = GLvector( 0.0,  ty ); vertices[0] = GLvector( -1.0, -1.0, z );
					texcoord[1] = GLvector( 1.0,  ty ); vertices[1] = GLvector(  1.0, -1.0, z );
					texcoord[2] = GLvector( 0.0, 0.0 ); vertices[2] = GLvector( -1.0,   qy, z );
					texcoord[3] = GLvector( 1.0, 0.0 ); vertices[3] = GLvector(  1.0,   qy, z );

					break;
			}

			glutPaintArrays( 4, vertices, texcoord );

			glMatrixMode( GL_MODELVIEW  ); glPopMatrix();
			glMatrixMode( GL_PROJECTION ); glPopMatrix();
			glPopAttrib();
		}


		void bind(GLenum where = GL_TEXTURE0 ) {
			if (m_bound) release();
			glActiveTexture( where );
			glEnable( TEXTURE_TARGET ); 
			glBindTexture( TEXTURE_TARGET, m_myname );
			m_bound = where;
		}

		void release() {
			if (!m_bound) return;
			glActiveTexture( m_bound );
			glBindTexture( TEXTURE_TARGET, 0 );
			glDisable( TEXTURE_TARGET );
			m_bound = 0;
		}

		GLuint   get() { return m_myname; }
		GLint  width( int orig = 0 ) { if (orig) return  m_width; return (TEXTURE_TARGET == GL_TEXTURE_2D) ? 1 :  m_width; }
		GLint height( int orig = 0 ) { if (orig) return m_height; return (TEXTURE_TARGET == GL_TEXTURE_2D) ? 1 : m_height; }


		void load( const GLvoid* data, GLenum data_format, GLenum data_type ) {
			bind( GL_TEXTURE0 );
			glTexImage2D( TEXTURE_TARGET, 0, TEXTURE_FORMAT, m_width, m_height, 0, data_format, data_type, data );
			release();
		}

		void load( const char* pngfile ) {
			PNGImage* tmp = new PNGImage( pngfile );
			m_width  = tmp->width();
			m_height = tmp->height();
			load( tmp->data(), GL_RGBA, GL_UNSIGNED_BYTE );
			delete tmp;
		}

		void load( const IntensityImage* img ) {
			m_width  = img->getWidth();
			m_height = img->getHeight();
			load( img->getData(), GL_LUMINANCE, GL_UNSIGNED_BYTE );
		}

		void load( const ShortImage* img ) {
			m_width  = img->getWidth();
			m_height = img->getHeight();
			load( img->getData(), GL_LUMINANCE, GL_UNSIGNED_SHORT );
		}

		void load( const RGBImage* img ) {
			m_width  = img->getWidth();
			m_height = img->getHeight();
			load( img->getData(), GL_RGB, GL_UNSIGNED_BYTE );
		}

		void read( GLvoid* data ) {
			bind( GL_TEXTURE0 );
			if (data) glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
			glGetTexImage( TEXTURE_TARGET, 0, DATA_FORMAT, DATA_TYPE, data );
		}


	protected:

		void init( GLint w, GLint h, GLenum filter = GL_LINEAR, GLenum mode = GL_REPLACE ) {

			m_bound  = 0;
			m_myname = 0;
			m_width  = w;
			m_height = h;
			
			glEnable( TEXTURE_TARGET );
			glGenTextures( 1, &m_myname );
			bind( GL_TEXTURE0 );

			glTexParameteri( TEXTURE_TARGET, GL_TEXTURE_MIN_FILTER, filter );
			glTexParameteri( TEXTURE_TARGET, GL_TEXTURE_MAG_FILTER, filter );

			glTexParameteri( TEXTURE_TARGET, GL_TEXTURE_WRAP_S, GL_CLAMP );
			glTexParameteri( TEXTURE_TARGET, GL_TEXTURE_WRAP_T, GL_CLAMP );

			glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode );

			load( 0, (DATA_FORMAT == GL_RGBA)? GL_BGRA : DATA_FORMAT, DATA_TYPE );
		}

		GLint m_width,m_height;
		GLuint m_myname;
		GLenum m_bound;

};


#if 0
typedef Texture < DEFAULT_TEXTURE_TARGET, GL_RGBA16,            GL_RGBA,            GL_UNSIGNED_SHORT > ShortTexture;
typedef Texture < DEFAULT_TEXTURE_TARGET, GL_RGBA32F_ARB,       GL_RGBA,            GL_FLOAT          > FloatTexture;
typedef Texture < DEFAULT_TEXTURE_TARGET, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT          > DepthTexture;
#endif

typedef Texture < DEFAULT_TEXTURE_TARGET, GL_RGB8,       GL_RGB,       GL_UNSIGNED_BYTE > RGBTexture;
typedef Texture < DEFAULT_TEXTURE_TARGET, GL_RGBA8,      GL_RGBA,      GL_UNSIGNED_BYTE > RGBATexture;
typedef Texture < DEFAULT_TEXTURE_TARGET, GL_LUMINANCE8, GL_LUMINANCE, GL_UNSIGNED_BYTE > GreyTexture;
typedef Texture < DEFAULT_TEXTURE_TARGET, GL_LUMINANCE16, GL_LUMINANCE, GL_UNSIGNED_SHORT > ShortGreyTexture;


#endif // _TEXTURE_H_

