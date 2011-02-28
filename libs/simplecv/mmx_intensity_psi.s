/* 
	compile with: as mmx_intensity_psi.s -o intel.o 
	register usage:
		eax: index   (0)
		ebx: result  (out, uint32_t)
		ecx: 2*count (in,  uint32_t)
		esi: data    (in,  uint16_t*)
*/

; .intel_syntax noprefix
; .globl mmx_intensity_psi
; .hidden mmx_intensity_psi
; mmx_intensity_psi:

xor   eax, eax
pxor  mm7, mm7
mov   ebx, 255
movd  mm2, ebx
movq  mm3, mm2
psllq mm3, 32
por   mm3, mm2

sintloop:

	movq  mm1, [esi+eax]
	psrlq mm1, 8
	movq  mm4, mm3
	pand  mm4, mm1
	paddd mm7, mm4

	psrlq mm1, 16
	movq  mm4, mm3
	pand  mm4, mm1
	paddd mm7, mm4

	add eax, 8
	cmp eax, ecx
	jb sintloop

movd  eax, mm7
psrlq mm7, 32
movd  ebx, mm7
add   ebx, eax

; ret

