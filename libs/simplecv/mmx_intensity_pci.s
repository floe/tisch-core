/* 
	compile with: as mmx_intensity_pci.s -o intel.o 
	register usage:
		eax: index  (0)
		ebx: result (out, uint32_t)
		ecx: count  (in,  uint32_t)
		esi: data   (in,  uint8_t*)
*/

; .intel_syntax noprefix
; .globl mmx_intensity_pci
; .hidden mmx_intensity_pci
; mmx_intensity_pci:

xor   eax, eax
pxor  mm7, mm7
mov   ebx, 255
movd  mm2, ebx
movq  mm3, mm2
psllq mm3, 32
por   mm3, mm2

cintloop:

	movq  mm1, [esi+eax]
	movq  mm4, mm3
	pand  mm4, mm1
	paddd mm7, mm4

	psrlq mm1, 8
	movq  mm4, mm3
	pand  mm4, mm1
	paddd mm7, mm4

	psrlq mm1, 8
	movq  mm4, mm3
	pand  mm4, mm1
	paddd mm7, mm4

	psrlq mm1, 8
	movq  mm4, mm3
	pand  mm4, mm1
	paddd mm7, mm4

	add eax, 8
	cmp eax, ecx
	jb cintloop

movd  eax, mm7
psrlq mm7, 32
movd  ebx, mm7
add   ebx, eax

; ret
