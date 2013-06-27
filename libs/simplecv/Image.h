/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _IMAGE_H_
#define _IMAGE_H_

#if __linux

	#include <fcntl.h>
	#include <unistd.h>
	
	#ifndef __ANDROID__
		#include <SmartPtr.h>
		#include <SharedMem.h>
		#include <sys/mman.h>
	#endif

	#include <linux/videodev2.h>

	#ifdef HAS_LIBV4L
		#include <libv4l2.h>
		#define mmap v4l2_mmap
	#endif

#elif __APPLE__

	#include <fcntl.h>
	#include <sys/shm.h>
	#include <sys/sem.h>
	#include <sys/mman.h>
	#include <sys/types.h>

#endif

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>

#include <stdint.h>
#include <string.h>
#include <errno.h>


class TISCH_SHARED Image {

	friend class ImageSource;

	public:

		Image( int _width, int _height, double _bpp, key_t _key = 0, unsigned long long _flags = 0 ) {
			init( _width, _height, _bpp, _key, _flags );
		}

		Image( const Image& img, key_t _key = 0, unsigned long long _flags = 0 ) {
			init( img.width, img.height, img.bpp, _key, _flags );
			memcpy( data, img.data, size );
		}

		// destructor to clean the freestore/IPC mess
		virtual ~Image() {
			if (key == 0) {
				// no key given, so simply delete the image
				delete[] rawdata;
			} else if (shm == NULL) {
				// we used mmap, so detach again
				munmap( data, size );
			} else {
				// delete the shared memory wrapper
				// delete shm; - will be done by SmartPtr
			}
		}

		Image& operator= ( const Image& img ) {
			if (this == &img) return *this;
			if (size != img.size) throw std::runtime_error( "Image: raw size mismatch in assignment" );
			memcpy( data, img.data, size );
			return *this;
		}

		void swapData( Image& img ) {
			if (size != img.size) throw std::runtime_error( "Image: raw size mismatch in buffer swap" );
			uint8_t* tmp = img.data; img.data = data; data = tmp;
			tmp = img.rawdata; img.rawdata = rawdata; rawdata = tmp;
		}

		inline unsigned char* getData() const { return data; }
		inline int getCount () const { return count;  }
		inline int getWidth () const { return width;  }
		inline int getHeight() const { return height; }

		inline void clear( unsigned char value = 0 ) { memset( data, value, size ); }

		int pixelOffset(int x, int y, int channel = 0) const { return 0; }
		unsigned char getPixel(int x, int y, int channel = 0) const { return 0; }
		void setPixel(int x, int y, unsigned char value, int channel = 0) { }

		inline void timestamp( unsigned long long int val ) { imgtime = val; }
		inline unsigned long long int timestamp() { return imgtime; }

		void acquire() { if (shm) shm->acquire(); }
		void release() { if (shm) shm->release(); }

	protected:

		// this one should never be used
		Image() { data = 0; }

		// constructors call init() to do the real construction work
		// _key  : IPC identifier or file descriptor for mmap
		// _flags: if IPC:
		//           0 -> attach to already existing segment
		//           1 -> create new segment with mode 0666
		//         if mmap:
		//           mmap offset
		void init( int _width, int _height, double _bpp, key_t _key, unsigned long long _flags ) {
		
			bpp    = (int)_bpp;
			width  = _width;
			height = _height;

			imgtime = 0;
			count   = width*height;
			size    = (int)(count*_bpp);

			shm = NULL;
			key = _key;
			rawdata = NULL;

			if (key == 0) {
				// no key given, so we don't use shared memory or mmap, only the freestore (32-byte-aligned for MMX/SSE)
				rawdata = data = new unsigned char[size+64];
				data = (unsigned char*)((unsigned long long)(rawdata+32) & (unsigned long long int)(0xFFFFFFFFFFFFFFE0ULL));
				// uncomment for debugging to make valgrind happy (doesn't know mmap)
				// for (int i = 0; i < size; i++) rawdata[i] = 0;
			}

			#ifdef __linux
			else if (fcntl( key, F_GETFL ) != -1) {
				// key is a file descriptor, so use mmap with flags == struct v4l2_buffer* giving offset & size
				struct v4l2_buffer* tmpbuf = (struct v4l2_buffer*)_flags;
				data = (unsigned char*) mmap( NULL, tmpbuf->length, PROT_READ | PROT_WRITE, MAP_SHARED, key, tmpbuf->m.offset );
				if (data == MAP_FAILED) throw std::runtime_error( std::string("mmap: ").append(strerror(errno)) );
			} 
			#endif

			else {
				// create new shared memory wrapper
				shm = new SharedMem( key, size, (_flags==1) );
				// attach segment 
				data = shm->get();
			}
		}

		void load( const char* path, const char* type, int maxbpp ) {

			int fwidth,fheight,fvalues;
			std::string magic,tmp;

			// open file with whitespace skipping
			std::ifstream myfile( path, std::ios::in );
			myfile >> std::skipws;

			// parse the header
			myfile >> magic;   myfile.ignore(1); if (myfile.peek() == '#') getline( myfile, tmp );
			myfile >> fwidth;  myfile.ignore(1); if (myfile.peek() == '#') getline( myfile, tmp );
			myfile >> fheight; myfile.ignore(1); if (myfile.peek() == '#') getline( myfile, tmp );
			myfile >> fvalues;

			if ((magic != type) || (fvalues >= (1<<(8*maxbpp))) || (fvalues < 1)) 
				throw std::runtime_error( std::string("Image::load( ") + std::string(path) + std::string(" ): no valid PGM file") );

			// init the base class
			init( fwidth, fheight, maxbpp, 0, 0 );

			// skip one byte, read the rest
			myfile.ignore( 1 );
			myfile.read( (char*)data, size );
			myfile.close( );
		}

		void save( const char* filename, const char* magic ) {

			std::ofstream imagefile( filename, std::ios::out );

			// file header 
			imagefile << magic << "\n";
			imagefile << "# CREATOR: libTISCH version 3.0\n";
			imagefile << width << " " << height << " ";
			imagefile << ((1<<(8*bpp))-1) << "\n"; // values per pixel

			// file content
			imagefile.write( (char*)data, size );

			// be polite and tidy up ;)
			imagefile.close();
		}

		// image parameters
		int width, height, bpp, size, count;
		unsigned long long int imgtime;

		// the actual image data
		unsigned char* rawdata;
		unsigned char* data;

		// shared image support
		SmartPtr<SharedMem> shm;
		int key;

};

#endif // _IMAGE_H_

