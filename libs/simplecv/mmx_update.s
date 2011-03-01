/*
	void mmxupdate( unsigned char* in, unsigned char* mask, unsigned short* out, ASMINT count )

	register usage:

		mm0: 8 bytes input data
		mm1: 8 bytes mask data
		mm2: 4 words with previous values
		mm3: word-converted input data
		mm4: word-converted mask data
		mm5: temporary register
		mm7: zero for byte/word unpacking

		esi: data   (in uint8_t*)
		edi: result (in uint16_t*)
		edx: mask   (in uint8_t*)
		ecx: count  (in uint32_t)
		eax: index  (0)
*/

; .intel_syntax noprefix
; .globl mmx_update
; .hidden mmx_update
; mmx_update:

xor  eax, eax
pxor mm7, mm7 /* load zero into mm7 */

mmx_update_loop:

	movq mm0, [esi+eax] /* load input data in mm0 */
	movq mm1, [edx+eax] /* load mask in mm1 */

	movq mm2, [edi+eax*2] /* load first four words into mm2 */

	movq mm3, mm0 
	punpcklbw mm3, mm7 /* word-converted input data in mm3 */

	movq mm4, mm1 
	punpcklbw mm4, mm4 /* word-converted mask data in mm4 */

	psrlw mm3, 1 
	paddusw mm3, mm2 /* result, add new value to original one in mm3 */

	movq mm5, mm2 
	psrlw mm5, 9 /* fraction of old value in mm5 */
	psubusw mm3, mm5 /* subtract from mm3 */

	movq mm5, mm2 
	pand mm5, mm4 /* masked original data in mm5 */
	pandn mm4, mm3 /* masked updated data in mm4 */
	por mm4, mm5 /* combine in mm4 */

	movq [edi+eax*2], mm4 /* store first four words from mm4 */


	movq mm2, [edi+8+eax*2] /* load next four words into mm2 */

	movq mm3, mm0 
	punpckhbw mm3, mm7 /* word-converted input data in mm3 */

	movq mm4, mm1 
	punpckhbw mm4, mm4 /* word-converted mask data in mm4 */

	psrlw mm3, 1 
	paddusw mm3, mm2 /* result, add new value to original one in mm3 */

	movq mm5, mm2 
	psrlw mm5, 9 /* fraction of old value in mm5 */
	psubusw mm3, mm5 /* subtract from mm3 */

	movq mm5, mm2 
	pand mm5, mm4 /* masked original data in mm5 */
	pandn mm4, mm3 /* masked updated data in mm4 */
	por mm4, mm5 /* combine in mm4 */

	movq [edi+8+eax*2], mm4 /* store next four words from mm4 */

	add eax, 8 /* FIXME: hardcoded step width */
	cmp eax, ecx 
	jb mmx_update_loop

emms
; ret
