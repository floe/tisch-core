/* 
	void mmx_subtract( unsigned char* in, unsigned short* sub, unsigned char* out, ASMINT count )
	register usage:
		esi: data1  (in, uint8_t*)
		edi: result (in, uint8_t*)
		edx: data2  (in, uint16_t*)
		ecx: count  (in, uint32_t)
		eax: index  (0)
*/

; .intel_syntax noprefix
; .globl mmx_subtract_pcpspci
; .hidden mmx_subtract_pcpspci
; mmx_subtract_pcpspci:

xor eax, eax

sub2loop:

	movq  mm0, [esi+eax]      /* load input data in mm0 */
	movq  mm1, [edx+eax*2]    /* load first 4 words in mm1 */
	psrlw mm1, 8              /* convert to bytes */
	movq  mm2, [edx+8+eax*2]  /* load next 4 words in mm2 */
	psrlw mm2, 8              /* convert to bytes */
	packuswb mm1, mm2         /* pack into single qword */
	psubusb  mm0, mm1         /* subtract */
	movq [edi+eax], mm0       /* store result in mm1 */

	add eax, 8                /* FIXME: fixed step width */
	cmp eax, ecx
	jb sub2loop

; ret
