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
		#include <sys/ipc.h>
		#include <sys/shm.h>
		#include <sys/sem.h>
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

#elif _MSC_VER

	#define key_t int

#endif

#include <stdexcept>
#include <iostream>

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
				
			}
			
			#if !defined(_MSC_VER) && !defined(__ANDROID__)

			else if (shm == 0) {

				// we used mmap, so detach again
				munmap( data, size );

			} else {

				struct shmid_ds shmstat;
				
				// first, detach the shared memory segment
				shmdt( (void*) data );
				
				// we created the IPC handles, so we have to clean up
				if (flags | IPC_CREAT) {
				  shmctl( shm, IPC_RMID, &shmstat );
				  semctl( sem, 0, IPC_RMID );
				}
			}

			#endif
		}

		Image& operator= ( const Image& img ) {
			if (this == &img) return *this;
			if (size != img.size) throw std::runtime_error( "Image: raw size mismatch in assignment" );
			memcpy( data, img.data, size );
			return *this;
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

		#if !defined(_MSC_VER) && !defined(__ANDROID__)

		inline int acquire() { return do_sem( -1 ); }
		inline int release() { return do_sem( +1 ); }

		#endif
		
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

			key    = _key;
			flags  = (int)_flags;

			shm = sem = 0;

			if (key == 0) {

				// no key given, so we don't use shared memory or mmap, only the freestore (32-byte-aligned for MMX/SSE)
				rawdata = data = new unsigned char[size+64];
				data = (unsigned char*)((unsigned long long)(rawdata+32) & (unsigned long long int)(0xFFFFFFFFFFFFFFE0ULL));
				// uncomment for debugging to make valgrind happy (doesn't know mmap)
				// for (int i = 0; i < size; i++) rawdata[i] = 0;

			}

			#if defined(_MSC_VER) || defined(__ANDROID__)

				else throw std::runtime_error( "Shared memory is currently unsupported on Android & Windows." );

			#else

			else if (fcntl( key, F_GETFL ) != -1) {

				#ifndef __APPLE__
				// key is a file descriptor, so use mmap with flags == struct v4l2_buffer* giving offset & size
				struct v4l2_buffer* tmpbuf = (struct v4l2_buffer*)_flags;
				data = (unsigned char*) mmap( NULL, tmpbuf->length, PROT_READ | PROT_WRITE, MAP_SHARED, key, tmpbuf->m.offset );
				if (data == MAP_FAILED) throw std::runtime_error( std::string("mmap: ").append(strerror(errno)) );
				#endif

			} else {
				
				// flag setting '1' doesn't make sense, replace with default values
			 	if (flags	== 1) flags = (IPC_CREAT | 0666);

				// only one of the images should use IPC_CREAT, so add IPC_EXCL that all other tries fail
				if (flags | IPC_CREAT) flags |= IPC_EXCL;

				// key is a IPC id, so create/retrieve segment and semaphore handles
				shm = shmget( key, size, flags );
				sem = semget( key,    1, flags );

				// attach segment and check for errors
				data = (unsigned char*) shmat( shm, 0, 0 );
				if (data == (void*)-1) throw std::runtime_error( std::string("shmat: ") + std::string(strerror(errno)) );
				
				// set semaphore value to 1 if it was newly created
				if (flags | IPC_CREAT) semctl( sem, 0, SETVAL, 1 );
			}

			#endif
		}

		#if !defined(_MSC_VER) && !defined(__ANDROID__)

		// wrapper for semaphore access
		inline int do_sem( short int val ) {
			struct sembuf sem_op = { 0, val, 0 };
			return semop( sem, &sem_op, 1 );
		}

		#endif
		
		// image parameters
		int width, height, bpp, size, count;
		unsigned long long int imgtime;

		// the actual image data
		unsigned char* rawdata;
		unsigned char* data;

		// shared image support
		int shm, sem, flags;
		key_t key;

};

#endif // _IMAGE_H_

