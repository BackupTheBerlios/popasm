cmps [si],[di]
bits 16
cmps byte ds:[si],es:[di]
cmps word ds:[esi],es:[edi]
cmps
cmps [bx],fs:[di]
bits 32
cmps word fs:[bx],es:[di]
cmps dword gs:[bx],[di]
cmps byte gs:[bx],[edi]
