/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/


#ifndef _MSC_VER // glibc replacement definitions for MSVC compiler

#include <unistd.h>

#else

#ifndef _NANOLIBC_H_
#define _NANOLIBC_H_

#include <tisch.h>

#include <process.h>
#include <float.h>
#include <math.h>

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

inline long long atoll( const char* in ) {
	long long ret = 0;
	while (*in) { ret = 10*ret + (*in - '0'); in++; }
	return ret;
}

#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
inline double round(double x) { return floor(x+0.5); }

#define M_PI 3.14159265358979323846

#define srandom srand
#define random rand

#define snprintf sprintf_s
#define usleep(x) Sleep(x/1000)
#define sleep(x) Sleep(x)

#define alarm(x) countdown(x)
#define execv(x,y) _execv(x,y)
#define fork() 0

#define SIGHUP  SIGTERM
#define SIGQUIT SIGINT
#define SIGALRM SIGTERM
#define SIGUSR1 SIGTERM

extern TISCH_SHARED char* optarg;
int TISCH_SHARED getopt( unsigned int argc, char* argv[], const char* opts );

#endif // _NANOLIBC_H_

#endif // _MSC_VER
