adc
adc ax,bx
adc cx,dl
adc eax,12
adc esp,-8
adc eax,word 12
adc eax,dword 12
adc ebx,byte 200
adc ebx,byte 127
adc ax,32
adc cl,[bx+si]
adc [bp+8],sp
adc bh,0Bh
adc [200h],5
adc byte [200h],5
adc byte [200h],byte 5
adc [200h],byte 5
adc byte [100h], word [200h]
adc al,oword [bx]
