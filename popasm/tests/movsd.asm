bits 16
movsd
bits 32
movsd
movsd ds:[si],es:[di]
movsd fs:[si],[di]

movsd xmm1,xmm2
movsd xmm3,[bx]
movsd xmm4,oword [bx]
movsd xmm4,qword [bx]
movsd [ecx],xmm7
movsd qword [ecx],xmm7
movsd oword [ecx],xmm7
