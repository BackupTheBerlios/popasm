lfs ax,5
lfs cx,bp
lfs word [di],ax
lfs ax,word [di]
lfs ax,dword [di]
lfs ax,pword [di]
lfs ax,[di]
lfs word [di],eax
lfs eax,word [di]
lfs eax,dword [di]
lfs eax,[di]
lds eax,pword [di]
lfs ds,ax

