bits 16
cmpsd
bits 32
cmpsd
cmpsd ds:[si],es:[di]
cmpsd fs:[si],[di]

cmpsd xmm1,xmm2,6
cmpsd xmm3,[bx],1
cmpsd xmm4,oword [bx],2
cmpsd [ecx],xmm7,0
