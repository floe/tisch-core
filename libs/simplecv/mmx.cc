/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdio.h>
#include "mmx.h"

// TODO: except for the despeckle routine,
// everything could be converted to SSE2..
// but 16 byte alignment might be a problem.

// amount of bytes in a single loop iteration: 8 for MMX, 16 for SSE(2)
#define STEP 8


// Note: there's a number of nasty hacks going on here. 
//
// The most important one is regarding the EBX (i386) or RBX (x86-64) register
// which stores the address of the global offset table (GOT) in position-
// independent code (PIC). As library code is more or less by definition PIC, 
// the BX register _MUST NOT_ be clobbered outside of the assembler functions 
// at any time.  However, registers are scarce, so the two macros ASMINIT and 
// ASMEXIT save and restore EBX/RBX _before_ the register loading code for the 
// main asm() block runs. On i386, this code can't be compiled with -fPIC, 
// otherwise gcc would complain about this dirty trick. On x86-64, we have 
// enough registers, so this is not such a big problem.
//
// The second hack is the ASMINT macro (defined in mmx.h) which represents an
// integer of the same size as the largest register. 
 
#ifdef __x86_64__

	#define STORAGE 
	#define ASMINIT "	pushq %%rbx\n"
	#define ASMEXIT "	popq  %%rbx\n	emms\n"

#elif __i586__ || __i686__ || __tune_i686__ || __i386__

	#define STORAGE 
	#define ASMINIT "	pushl %%ebx\n"
	#define ASMEXIT "	popl  %%ebx\n	emms\n"

#elif _M_X64

	#define STORAGE

	#define pax rax
	#define pbx rbx
	#define pcx rcx
	#define pdx rdx
	#define psi rsi
	#define pdi rdi

#elif _M_IX86

	#define STORAGE

	#define pax eax
	#define pbx ebx
	#define pcx ecx
	#define pdx edx
	#define psi esi
	#define pdi edi

#endif


int mmxintensity( unsigned char* in, ASMINT count ) {

	STORAGE ASMINT start = 0;
	STORAGE int tmp = 0x12345678;

#ifdef _MSC_VER

	__asm {
	mov psi, in
	mov pcx, count
	mov pax, start
	#include "mmx_intensity_c.sx"
	mov tmp, eax
	}

#else

	asm(

		/*ASMINIT

		"	pxor %%mm7, %%mm7            \n" // zero in mm7
		"	movl $255, %%ebx             \n"
		"	movd %%ebx, %%mm2            \n"
		" movq %%mm2, %%mm3            \n"
		" psllq $32, %%mm3             \n"
		" por %%mm2, %%mm3             \n" // mask 0x000000FF * 2 in mm3

		"	cintloop:                    \n"

		"		movq (%[in],%[idx]), %%mm1 \n"
		"		movq %%mm3, %%mm4          \n"
		"		pand %%mm1, %%mm4          \n"
		"		paddd %%mm4, %%mm7         \n"

		"		psrlq $8, %%mm1            \n"
		"		movq %%mm3, %%mm4          \n"
		"		pand %%mm1, %%mm4          \n"
		"		paddd %%mm4, %%mm7         \n"

		"		psrlq $8, %%mm1            \n"
		"		movq %%mm3, %%mm4          \n"
		"		pand %%mm1, %%mm4          \n"
		"		paddd %%mm4, %%mm7         \n"

		"		psrlq $8, %%mm1            \n"
		"		movq %%mm3, %%mm4          \n"
		"		pand %%mm1, %%mm4          \n"
		"		paddd %%mm4, %%mm7         \n"

		"		add %[inc],%[idx]          \n"
		"		cmp %[cnt],%[idx]          \n"
		"		jb cintloop                \n"

		"	movd %%mm7, %%eax            \n"
		"	psrlq $32, %%mm7             \n"
		"	movd %%mm7, %%ebx            \n"
		"	addl %%eax, %%ebx            \n"
		"	movl %%ebx, %[out]           \n"

		ASMEXIT*/

		"call mmx_intensity_c \n"
		"movl %%eax, %[out]   \n"

		: [out] "=m" (tmp)
		: [in]  "S" (in),
		  [cnt] "c" (count),
		  [idx] "a" (start)

	);

#endif

	return tmp/count;
}


int mmxintensity( unsigned short* in, ASMINT count ) {

	STORAGE ASMINT start = 0;
	STORAGE ASMINT count2 = 2*count;
	STORAGE int tmp = 0x12345678;

#ifdef _MSC_VER

	__asm {
	mov psi, in
	mov pcx, count2
	mov pax, start
	#include "mmx_intensity_s.sx"
	mov tmp, eax
	}

#else

	asm(

		/*ASMINIT

		"	pxor %%mm7, %%mm7            \n" // zero in mm7
		"	movl $255, %%ebx             \n"
		"	movd %%ebx, %%mm2            \n"
		" movq %%mm2, %%mm3            \n"
		" psllq $32, %%mm3             \n"
		" por %%mm2, %%mm3             \n" // mask 0x000000FF * 2 in mm3

		"	sintloop:                    \n"

		"		movq (%[in],%[idx]), %%mm1 \n"
		"		psrlq $8, %%mm1            \n"
		"		movq %%mm3, %%mm4          \n"
		"		pand %%mm1, %%mm4          \n"
		"		paddd %%mm4, %%mm7         \n"

		"		psrlq $16, %%mm1           \n"
		"		movq %%mm3, %%mm4          \n"
		"		pand %%mm1, %%mm4          \n"
		"		paddd %%mm4, %%mm7         \n"

		"		add %[inc],%[idx]          \n"
		"		cmp %[cnt],%[idx]          \n"
		"		jb sintloop                \n"

		"	movd %%mm7, %%eax            \n"
		"	psrlq $32, %%mm7             \n"
		"	movd %%mm7, %%ebx            \n"
		"	addl %%eax, %%ebx            \n"
		"	movl %%ebx, %[out]           \n"

		ASMEXIT*/

		"call mmx_intensity_s \n"
		"movl %%eax, %[out]   \n"

		: [out] "=m" (tmp)
		: [in]  "S" (in),
		  [cnt] "c" (count2),
		  [idx] "a" (start)

	);

#endif

	return tmp/count;
}


void mmxsubtract( unsigned short* sub, unsigned char* in, unsigned char* out, ASMINT count ) {

#ifdef _MSC_VER

	__asm {
		mov psi, in
		mov pdi, out
		mov pdx, sub
		mov pcx, count
		#include "mmx_subtract_sc.sx"
	}

#else

	asm(

		/*ASMINIT

		"	sub1loop:                        \n"

		"		movq  ( %[in],%[idx]),   %%mm0 \n" // load input data in mm0
		"		movq  (%[sub],%[idx],2), %%mm1 \n" // load first 4 words in mm1
		"		psrlw $8, %%mm1                \n" // convert to bytes
		"		movq 8(%[sub],%[idx],2), %%mm2 \n" // load next 4 words in mm2
		"		psrlw $8, %%mm2                \n" // convert to bytes
		"		packuswb %%mm2, %%mm1          \n" // pack into single qword
		"		psubusb %%mm0, %%mm1           \n" // subtract
		"		movq %%mm1, (%[out],%[idx])    \n" // store result in mm1

		"		add     $8,%[idx]          \n"
		"		cmp %[cnt],%[idx]          \n"
		"		jb sub1loop                \n"

		ASMEXIT */

		"call mmx_subtract_sc \n"

		::[in]  "S" (in),
		  [out] "D" (out),
		  [sub] "d" (sub),
		  [cnt] "c" (count)

	);
#endif
}

void mmxsubtract( unsigned char* in, unsigned short* sub, unsigned char* out, ASMINT count ) {

#ifdef _MSC_VER

	__asm {
		mov psi, in
		mov pdi, out
		mov pdx, sub
		mov pcx, count
		#include "mmx_subtract_cs.sx"
	}

#else

	asm(

		/*ASMINIT

		"	sub2loop:                        \n"

		"		movq  ( %[in],%[idx]),   %%mm0 \n" // load input data in mm0
		"		movq  (%[sub],%[idx],2), %%mm1 \n" // load first 4 words in mm1
		"		psrlw $8, %%mm1                \n" // convert to bytes
		"		movq 8(%[sub],%[idx],2), %%mm2 \n" // load next 4 words in mm2
		"		psrlw $8, %%mm2                \n" // convert to bytes
		"		packuswb %%mm2, %%mm1          \n" // pack into single qword
		"		psubusb %%mm1, %%mm0           \n" // subtract
		"		movq %%mm0, (%[out],%[idx])    \n" // store result in mm0

		"		add %[inc],%[idx]          \n"
		"		cmp %[cnt],%[idx]          \n"
		"		jb sub2loop                \n"

		ASMEXIT*/

		"call mmx_subtract_cs \n"

		::[in]  "S" (in),
		  [out] "D" (out),
		  [sub] "d" (sub),
		  [cnt] "c" (count)

	);
#endif
}


void mmxupdate( unsigned char* in, unsigned char* mask, unsigned short* out, ASMINT count ) {

	/* register usage:
	 *   mm0: 8 bytes input data
	 *   mm1: 8 bytes mask data
	 *   mm2: 4 words with previous values
	 *   mm3: word-converted input data
	 *   mm4: word-converted mask data
	 *   mm5: temporary register
	 *   mm7: zero for byte/word unpacking
	*/

#ifdef _MSC_VER

	__asm {
		mov psi, in
		mov pdi, out
		mov pdx, mask
		mov pcx, count
		#include "mmx_update.sx"
	}

#else

	asm(

		/*ASMINIT

		" pxor %%mm7, %%mm7                \n" // load zero into mm7

		"	updloop:                         \n"

		"		movq (  %[in],%[idx]), %%mm0   \n" // load input data in mm0
		"		movq (%[mask],%[idx]), %%mm1   \n" // load mask in mm1

		"		movq (%[out],%[idx],2), %%mm2  \n" // load first four words into mm2

		"		movq %%mm0, %%mm3              \n"
		"		punpcklbw %%mm7, %%mm3         \n" // word-converted input data in mm3

		"		movq %%mm1, %%mm4              \n"
		"		punpcklbw %%mm4, %%mm4         \n" // word-converted mask data in mm4

		"		psrlw $1, %%mm3                \n"
		"		paddusw %%mm2, %%mm3           \n" // add new value to original one, result in mm3

		"		movq %%mm2, %%mm5              \n"
		"		psrlw $9, %%mm5                \n" // fraction of old value in mm5
		"		psubusw %%mm5, %%mm3           \n" // subtract from mm3

		"		movq %%mm2, %%mm5              \n"
		"		pand %%mm4, %%mm5              \n" // masked original data in mm5
		"		pandn %%mm3, %%mm4             \n" // masked updated data in mm4
		"		por %%mm5, %%mm4               \n" // combine in mm4

		"		movq %%mm4, (%[out],%[idx],2)  \n" // store first four words from mm4


		"		movq 8(%[out],%[idx],2), %%mm2 \n" // load next four words into mm2

		"		movq %%mm0, %%mm3              \n"
		"		punpckhbw %%mm7, %%mm3         \n" // word-converted input data in mm3

		"		movq %%mm1, %%mm4              \n"
		"		punpckhbw %%mm4, %%mm4         \n" // word-converted mask data in mm4

		"		psrlw $1, %%mm3                \n"
		"		paddusw %%mm2, %%mm3           \n" // add new value to original one, result in mm3

		"		movq %%mm2, %%mm5              \n"
		"		psrlw $9, %%mm5                \n" // fraction of old value in mm5
		"		psubusw %%mm5, %%mm3           \n" // subtract from mm3

		"		movq %%mm2, %%mm5              \n"
		"		pand %%mm4, %%mm5              \n" // masked original data in mm5
		"		pandn %%mm3, %%mm4             \n" // masked updated data in mm4
		"		por %%mm5, %%mm4               \n" // combine in mm4

		"		movq %%mm4, 8(%[out],%[idx],2) \n" // store next four words from mm4

		"		add %[inc],%[idx]          \n"
		"		cmp %[cnt],%[idx]          \n"
		"		jb updloop                 \n"

		ASMEXIT*/

		"call mmx_update \n"

		::[in]   "S" (in),
		  [out]  "D" (out),
		  [mask] "d" (mask),
		  [cnt]  "c" (count)

	);
#endif
}


void mmxthreshold( unsigned char* in, unsigned char* out, ASMINT count, unsigned char lower, unsigned char upper ) {

	/* //warning: this is SSE2 code, so it requires a P4 or above

	if (count % 16) return;

	ASMINT start = 0;
	ASMINT inc   = 16;
	ASMINT thr   = thresh;

	asm(
		"	movd %[thr], %%xmm1              \n" // 16 bytes with threshold in mm1
		"	punpcklbw %%xmm1, %%xmm1         \n"
		"	punpcklbw %%xmm1, %%xmm1         \n"
		"	punpcklbw %%xmm1, %%xmm1         \n"
		"	punpcklbw %%xmm1, %%xmm1         \n"
		"	movd %[inc], %%xmm2              \n" // 16 bytes with MSB set in mm2
		"	psllw $3, %%xmm2                 \n"
		"	punpcklbw %%xmm2, %%xmm2         \n"
		"	punpcklbw %%xmm2, %%xmm2         \n"
		"	punpcklbw %%xmm2, %%xmm2         \n"
		"	punpcklbw %%xmm2, %%xmm2         \n"
		"	pxor %%xmm2, %%xmm1              \n" // convert unsigned->signed by flipping each MSB
		"	thrloop:                         \n"
		"		movapd (%[in],%[idx]), %%xmm0  \n" // load 16 bytes to mm0
		"		pxor %%xmm2, %%xmm0            \n" // convert unsigned->signed by flipping each MSB
		"		pcmpgtb %%xmm1, %%xmm0         \n" // signed compare of mm1 and mm0
		"		movapd %%xmm0, (%[out],%[idx]) \n" // store 16 bytes from mm0
				ASMLOOP
		"		jb thrloop                     \n"
		"	emms                             \n"
		::[in]  "S" (in),
		  [out] "D" (out),
		  [cnt] "c" (count),
		  [idx] "a" (start),
		  [thr] "m" (thr),
		  [inc] "d" (inc)
	);*/

	STORAGE ASMINT lthr = lower;
	STORAGE ASMINT uthr = upper;

#ifdef _MSC_VER

	__asm {
		mov psi, in
		mov pdi, out
		mov pax, uthr
		mov pcx, count
		mov pdx, lthr
		#include "mmx_threshold.sx"
	}

#else

	asm(

		/* ASMINIT

		"	movd %[thr], %%mm1            \n" // 8 bytes with threshold in mm1
		"	punpcklbw %%mm1, %%mm1        \n"
		"	punpcklbw %%mm1, %%mm1        \n"
		"	punpcklbw %%mm1, %%mm1        \n"
		"	movd %[inc], %%mm2            \n" // 8 bytes with MSB set in mm2
		"	psllw $4, %%mm2               \n" // FIXME: relies on STEP == 8
		"	punpcklbw %%mm2, %%mm2        \n"
		"	punpcklbw %%mm2, %%mm2        \n"
		"	punpcklbw %%mm2, %%mm2        \n"
		"	pxor %%mm2, %%mm1             \n" // convert unsigned->signed by flipping each MSB
		"	thrloop:                      \n"
		"		movq (%[in],%[idx]), %%mm0  \n" // load 8 bytes to mm0
		"		pxor %%mm2, %%mm0           \n" // convert unsigned->signed by flipping each MSB
		"		pcmpgtb %%mm1, %%mm0        \n" // signed compare of mm1 and mm0
		"		movq %%mm0, (%[out],%[idx]) \n" // store 8 bytes from mm0
		"		add %[inc],%[idx]           \n"
		"		cmp %[cnt],%[idx]           \n"
		"		jb thrloop                  \n"

		ASMEXIT*/

		"call mmx_threshold\n"

		::[in]  "S" (in),
		  [out] "D" (out),
		  [cnt] "c" (count),
		  [upp] "a" (uthr),
		  [low] "d" (lthr)

	);
#endif
}


void mmxdespeckle( unsigned char* in, unsigned char* out, ASMINT linecnt, ASMINT stride, unsigned char thr ) {

	/* register usage:
	 *   mm0,1,2: previous, current, next row
	 *   mm3: 8 * 1 (LSB bitmask)
	 *   mm4: neighbor count accumulator
	 *   mm5: temporary register
	 *   mm6: 8 * (neighbor count threshold)
	*/

	STORAGE ASMINT count = linecnt * stride;
	STORAGE ASMINT thresh = thr;

#ifdef _MSC_VER

	__asm {
		mov psi, in
		mov pdi, out
		mov pax, count
		mov pcx, thresh
		mov pdx, stride
		#include "mmx_despeckle.sx"
	}

#else

	asm(

		/* ASMINIT

		//" movl %[thr], %%ebx          \n" // generate threshold values
		//" movl $7, %%ebx              \n"
		" movd %[thr], %%mm6            \n" // generate threshold values
		"	punpcklbw %%mm6, %%mm6        \n"
		"	punpcklbw %%mm6, %%mm6        \n"
		"	punpcklbw %%mm6, %%mm6        \n"

		" movl $1, %%ebx                \n" // generate bitmask with LSB set
		" movd %%ebx, %%mm3             \n"
		"	punpcklbw %%mm3, %%mm3        \n"
		"	punpcklbw %%mm3, %%mm3        \n"
		"	punpcklbw %%mm3, %%mm3        \n"

		" movq (%[in],%[idx]), %%mm0    \n" // preload first row
		" pand %%mm3, %%mm0             \n"
		" add  %[inc], %[idx]           \n"

		" mov  %[idx], %[thr]           \n" // reuse threshold register as output index

		" movq (%[in],%[idx]), %%mm1    \n" // preload second row
		" pand %%mm3, %%mm1             \n"
		" add  %[inc], %[idx]           \n"

		"	dsploop:                      \n"

		"		movq (%[in],%[idx]), %%mm2  \n" // load next row
		" 	pand %%mm3, %%mm2           \n"

		"		pxor %%mm4, %%mm4           \n" // clear the accumulator

		"		paddb %%mm0, %%mm4          \n" // add 8 neighborhood values
		"		paddb %%mm2, %%mm4          \n"

		"   movq %%mm0, %%mm5           \n"
		"		psrlq $8, %%mm5             \n"
		"		paddb %%mm5, %%mm4          \n"

		"   movq %%mm0, %%mm5           \n"
		"		psllq $8, %%mm5             \n"
		"		paddb %%mm5, %%mm4          \n"

		"   movq %%mm1, %%mm5           \n"
		"		psrlq $8, %%mm5             \n"
		"		paddb %%mm5, %%mm4          \n"

		"   movq %%mm1, %%mm5           \n"
		"		psllq $8, %%mm5             \n"
		"		paddb %%mm5, %%mm4          \n"

		"   movq %%mm2, %%mm5           \n"
		"		psrlq $8, %%mm5             \n"
		"		paddb %%mm5, %%mm4          \n"

		"   movq %%mm2, %%mm5           \n"
		"		psllq $8, %%mm5             \n"
		"		paddb %%mm5, %%mm4          \n"

		"		pcmpgtb %%mm6, %%mm4        \n" // compare accumulator against threshold

		"   psrlq $8, %%mm4             \n" // shift results (6 inner bytes) to the right
		                                    // 2 junk bytes are overwritten by next column

		"		movq %%mm4, 1(%[out],%[thr])\n" // store back to saved location + 1
		" 	add  %[inc], %[thr]         \n"

		"   movq %%mm1, %%mm0           \n" // cycle previous/current/next row values
		"   movq %%mm2, %%mm1           \n"

		"		add  %[inc], %[idx]         \n" // next loop iteration?
		"		cmp  %[cnt], %[idx]         \n"
		"		jb dsploop                  \n"

		ASMEXIT */

		"call mmx_despeckle\n"

		::[in]  "S" (in),
		  [out] "D" (out),
		  [cnt] "a" ((ASMINT)count),
		  [inc] "d" (stride),
		  [thr] "c" ((ASMINT)thresh)

	);
#endif
}
