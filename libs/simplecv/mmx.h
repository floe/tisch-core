/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _MMX_H_
#define _MMX_H_


#if __x86_64__ || _M_X64

	#define ASMINT long long int

#elif __i586__ || __i686__ || __tune_i686__ || __i386__ || _M_IX86

	#define ASMINT int

#else

	#define ASMINT int
	//#error Architecture not supported: i586, i686 or x86_64 required.
	#define NOMMX 1

#endif


void mmxsubtract( unsigned short* sub, unsigned char* in, unsigned char* out, ASMINT count );
void mmxsubtract( unsigned char* in, unsigned short* sub, unsigned char* out, ASMINT count );
void mmxupdate( unsigned char* in, unsigned char* mask, unsigned short* out, ASMINT count );

void mmxthreshold( unsigned char* in, unsigned char* out, ASMINT count, unsigned char lower, unsigned char upper );
void mmxdespeckle( unsigned char* in, unsigned char* out, ASMINT linecnt, ASMINT stride, unsigned char thresh );

int mmxintensity( unsigned  char* in, ASMINT count );
int mmxintensity( unsigned short* in, ASMINT count );


#endif // _MMX_H_
