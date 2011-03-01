/* 
	void mmxthreshold( unsigned char* in, unsigned char* out, ASMINT count, unsigned char lower, unsigned char upper)

	register usage:
		esi: data   (in, uint8_t*)
		edi: result (in, uint8_t*)
		eax: upper  (in, uint8_t), later used as index (0)
		edx: lower  (in, uint8_t)
		ecx: count  (in, uint32_t)

*/

; .intel_syntax noprefix
; .globl mmx_threshold
; .hidden mmx_threshold
; mmx_threshold:

movd mm3, eax  /* 8 bytes with upper threshold in mm3 */
punpcklbw mm3, mm3
punpcklbw mm3, mm3
punpcklbw mm3, mm3

xor eax, eax   /* eax now used as index register, starts at 0 */

movd mm1, edx  /* 8 bytes with lower threshold in mm1 */
punpcklbw mm1, mm1
punpcklbw mm1, mm1
punpcklbw mm1, mm1

mov  edx, 128
movd mm2, edx  /* 8 bytes with MSB set in mm2 */
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

emms
; ret

