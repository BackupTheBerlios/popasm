mov ax,bx
mov cx,dl
mov [bx+5],sp
mov bh,[100h]
mov byte [100h],byte [200h]
mov edx,[bp-16]
mov ax,ds
mov ds,ax
mov es,[ds:2Ch]
mov [1234h],GS
mov ax,[1234h]
mov [500h],eax
mov ch,50h
mov edx,byte -12h
mov [100h],5
mov word [100h],5
mov 50,ax
mov ax,cr0
mov cr0,eax
mov edx,dr6
mov dr5,[100h]
mov tr4,ebp
mov ebx,tr7
