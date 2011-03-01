/*
	void mmxdespeckle( unsigned char* in, unsigned char* out, ASMINT linecnt, ASMINT stride, unsigned char thresh )

	register usage:
		mm0,1,2: previous,current,next row
		mm3: 8 * 1 (LSB bitmask)
		mm4: neighbor count accumulator
		mm5: temporary register
		mm6: 8 * (neighbor count threshold)

		esi: data (in)
		edi: result (out)
		[cnt] no, "m" (count = linecnt * stride) grr. right now way to avoid memory access here (should use BX register!)
		eax: index (0)
		edx: stride
		ecx: thresh / output index
*/

; .intel_syntax noprefix
; .globl mmx_despeckle_pcpciic
; .hidden mmx_despeckle_pcpciic
; mmx_despeckle_pcpciic:

mov ebx, eax   /* move count to ebx */
xor eax, eax   /* set index to 0    */

movd mm6, ecx  /* generate threshold values */
punpcklbw mm6, mm6
punpcklbw mm6, mm6
punpcklbw mm6, mm6

mov  ebx, 1  /* generate bitmask with LSB set */
movd mm3, ebx
punpcklbw mm3, mm3
punpcklbw mm3, mm3
punpcklbw mm3, mm3

movq mm0, [esi+eax] /* preload first row */
pand mm0, mm3
add eax, edx

mov ecx, eax /* reuse threshold register as output index */

movq mm1, [esi+eax] /* preload second row */
pand mm1, mm3
add eax, edx

dsploop:

	movq mm2, [esi+eax] /* load next row */
	pand mm2, mm3

	pxor mm4, mm4  /* clear the accumulator */

	paddb mm4, mm0 /* add 8 neighborhood values */
	paddb mm4, mm2

	movq mm5, mm0
	psrlq mm5, 8
	paddb mm4, mm5

	movq mm5, mm0
	psllq mm5, 8
	paddb mm4, mm5

	movq mm5, mm1
	psrlq mm5, 8
	paddb mm4, mm5

	movq mm5, mm1
	psllq mm5, 8
	paddb mm4, mm5

	movq mm5, mm2
	psrlq mm5, 8
	paddb mm4, mm5

	movq mm5, mm2
	psllq mm5, 8
	paddb mm4, mm5

	pcmpgtb mm4, mm6 /* compare accumulator against threshold */

	psrlq mm4, 8 /* shift results (6 inner bytes) to the right  */
	             /* 2 junk bytes are overwritten by next column */

	movq [edi+ecx+1], mm4 /* store back to saved location + 1 */
	add ecx, edx

	movq mm0, mm1 /* cycle previous/current/next row values */
	movq mm1, mm2

	add eax, edx  /* next loop iteration? */
	cmp eax, ebx
	jb dsploop

; ret

