pshuflw xmm5,xmm,00_01_10_11B
pshuflw xmm2,mm7,10_01_11_00B
pshuflw xmm0,edx,255
pshuflw xmm3,dword [bx],word 5
pshuflw xmm7,qword [di],byte 40h
pshuflw xmm1,oword [bp],128

