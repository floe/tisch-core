/* 
	void mmxthreshold( unsigned char* in, unsigned char* out, ASMINT count, unsigned char thresh )

	register usage:
		esi: data
		edi: result
		edx: (lower) threshold
		ecx: count
		eax: index

	// example for GCC inline SSE2 code (requires P4 or above)

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
	);
	
*/

; .intel_syntax noprefix
; .globl mmx_threshold
; .hidden mmx_threshold
; mmx_threshold:

movd mm1, edx /* 8 bytes with threshold in mm1 */
punpcklbw mm1, mm1
punpcklbw mm1, mm1
punpcklbw mm1, mm1

movd  mm2, 8  /* 8 bytes with MSB set in mm2 */
psllw mm2, 4
punpcklbw mm2, mm2
punpcklbw mm2, mm2
punpcklbw mm2, mm2

pxor mm1, mm2 /* convert unsigned->signed by flipping each MSB */

mmx_threshold_loop:
	movq mm0, [esi+eax]  /* load 8 bytes to mm0 */
	pxor mm0, mm2        /* convert unsigned->signed by flipping each MSB */
	pcmpgtb mm0, mm1     /* signed compare of mm1 and mm0 */
	movq [edi+eax], mm0  /* store 8 bytes from mm0  */
	add eax,   8         /* FIXME: fixed step width */
	cmp eax, ecx
	jb mmx_threshold_loop

; ret

