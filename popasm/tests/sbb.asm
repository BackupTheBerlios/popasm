sbb
sbb ax,bx
sbb cx,dl
sbb eax,12
sbb esp,-8
sbb eax,word 12
sbb eax,dword 12
sbb ebx,byte 200
sbb ebx,byte 127
sbb ax,32
sbb cl,[bx+si]
sbb [bp+8],sp
sbb bh,0Bh
sbb [200h],5
sbb byte [200h],5
sbb byte [200h],byte 5
sbb [200h],byte 5
