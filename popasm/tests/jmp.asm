jmp short 53h
jmp 1234h
jmp dword 50h
jmp ax
jmp [bp]
jmp 1234h:5678h
jmp 1234h:5678ABCDh
jmp 1234h:123456789ABCh
jmp dword 1234h:56789ABCh
jmp 1234h:dword 56789ABCh
jmp [bx+si+34]
jmp far [bx+si+34]
jmp dword far [eax-12]
jmp far dword [eax-12]
