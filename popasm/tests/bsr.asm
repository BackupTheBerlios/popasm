bsr ax,5
bsr cx,bp
bsr word [di],ax
bsr ax,word [di]
bsr ax,[di]
bsr ds,ax

